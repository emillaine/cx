#include "typecheck.h"
#pragma warning(push, 0)
#include <llvm/Support/SaveAndRestore.h>
#pragma warning(pop)
#include "../ast/module.h"

using namespace cx;

void Typechecker::checkReturnPointerToLocal(const Expr* returnValue) const {
    if (auto* unaryExpr = llvm::dyn_cast<UnaryExpr>(returnValue)) {
        if (unaryExpr->getOperator() == Token::And) {
            returnValue = &unaryExpr->getOperand();
        }
    }

    Type localVariableType;
    const Expr* operand = returnValue;

    if (auto implicitCastExpr = llvm::dyn_cast<ImplicitCastExpr>(returnValue)) {
        operand = implicitCastExpr->getOperand();
    }

    if (auto varExpr = llvm::dyn_cast<VarExpr>(operand)) {
        switch (varExpr->getDecl()->kind) {
        case DeclKind::VarDecl: {
            auto* varDecl = llvm::cast<VarDecl>(varExpr->getDecl());
            if (varDecl->parent && varDecl->parent->isFunctionDecl()) {
                localVariableType = varDecl->type;
            }
            break;
        }
        case DeclKind::ParamDecl:
            localVariableType = llvm::cast<ParamDecl>(varExpr->getDecl())->type;
            break;

        default:
            break;
        }
    }

    if (localVariableType && currentFunction->getReturnType().removeOptional().isPointerType()
        && currentFunction->getReturnType().removeOptional().getPointee().equalsIgnoreTopLevelMutable(localVariableType)) {
        WARN(returnValue->getLocation(), "returning pointer to local variable (local variables will not exist after the function returns)");
    }
}

void Typechecker::typecheckReturnStmt(ReturnStmt& stmt) {
    Type returnValueType = stmt.value ? typecheckExpr(*stmt.value, false, currentFunction->getReturnType()) : Type::getVoid();

    if (!currentFunction->getReturnType()) {
        ASSERT(currentFunction->isLambda());
        currentFunction->proto.returnType = returnValueType;
    }

    if (!stmt.value) {
        if (!currentFunction->getReturnType().isVoid()) {
            ERROR(stmt.location, "expected return statement to return a value of type '" << currentFunction->getReturnType() << "'");
        }
        return;
    }

    if (auto converted = convert(stmt.value, currentFunction->getReturnType())) {
        stmt.value = converted;
    } else {
        ERROR(stmt.location, "mismatching return type '" << returnValueType << "', expected '" << currentFunction->getReturnType() << "'");
    }

    checkReturnPointerToLocal(stmt.value);
    setMoved(stmt.value, true);
}

void Typechecker::typecheckVarStmt(VarStmt& stmt) {
    typecheckVarDecl(*stmt.decl);
}

void Typechecker::typecheckIfStmt(IfStmt& ifStmt) {
    Type conditionType = typecheckExpr(*ifStmt.condition);
    typecheckImplicitlyBoolConvertibleExpr(conditionType, ifStmt.condition->location);
    currentControlStmts.push_back(&ifStmt);

    {
        llvm::SaveAndRestore thenMovedDecls(movedDecls);
        for (auto& stmt : ifStmt.thenBody) {
            typecheckStmt(stmt);
        }
    }

    {
        llvm::SaveAndRestore elseMovedDecls(movedDecls);
        for (auto& stmt : ifStmt.elseBody) {
            typecheckStmt(stmt);
        }
    }

    currentControlStmts.pop_back();
}

void Typechecker::typecheckSwitchStmt(SwitchStmt& stmt) {
    Type conditionType = typecheckExpr(*stmt.condition);

    if (!conditionType.isInteger() && !conditionType.isChar() && !conditionType.isEnumType()) {
        ERROR(stmt.condition->location, "switch condition must have integer, char, or enum type, got '" << conditionType << "'");
    }

    currentControlStmts.push_back(&stmt);

    for (auto& switchCase : stmt.cases) {
        Type caseType = typecheckExpr(*switchCase.value);

        if (auto converted = convert(switchCase.value, conditionType)) {
            switchCase.value = converted;
        } else {
            ERROR(switchCase.value->location, "case value type '" << caseType << "' doesn't match switch condition type '" << conditionType << "'");
        }

        Scope scope(nullptr, &currentModule->getSymbolTable());

        if (auto* associatedValue = switchCase.associatedValue) {
            auto* enumCase = llvm::cast<EnumCase>(llvm::cast<MemberExpr>(switchCase.value)->getDecl());
            associatedValue->type = NOTNULL(enumCase->associatedType);
            typecheckVarDecl(*associatedValue);
        }

        for (auto& caseStmt : switchCase.stmts) {
            typecheckStmt(caseStmt);
        }
    }

    for (auto& defaultStmt : stmt.defaultStmts) {
        typecheckStmt(defaultStmt);
    }

    currentControlStmts.pop_back();
}

void Typechecker::typecheckForStmt(ForStmt& forStmt) {
    Scope scope(currentFunction, &currentModule->getSymbolTable());

    if (forStmt.variable) {
        typecheckVarStmt(*forStmt.variable);
    }

    if (forStmt.condition) {
        Type conditionType = typecheckExpr(*forStmt.condition);
        typecheckImplicitlyBoolConvertibleExpr(conditionType, forStmt.condition->location);
    }

    currentControlStmts.push_back(&forStmt);

    for (auto& stmt : forStmt.body) {
        typecheckStmt(stmt);
    }

    currentControlStmts.pop_back();

    if (auto* increment = forStmt.increment) {
        typecheckExpr(*increment);
    }
}

void Typechecker::typecheckBreakStmt(BreakStmt& breakStmt) {
    if (llvm::none_of(currentControlStmts, [](const Stmt* stmt) { return stmt->isBreakable(); })) {
        ERROR(breakStmt.location, "'break' is only allowed inside 'while', 'for', and 'switch' statements");
    }
}

void Typechecker::typecheckContinueStmt(ContinueStmt& continueStmt) {
    if (llvm::none_of(currentControlStmts, [](const Stmt* stmt) { return stmt->isContinuable(); })) {
        ERROR(continueStmt.location, "'continue' is only allowed inside 'while' and 'for' statements");
    }
}

void Typechecker::typecheckCompoundStmt(CompoundStmt& compoundStmt) {
    Scope scope(currentFunction, &currentModule->getSymbolTable());

    for (auto& stmt : compoundStmt.body) {
        typecheckStmt(stmt);
    }
}

bool Typechecker::typecheckStmt(Stmt*& stmt) {
    try {
        switch (stmt->kind) {
        case StmtKind::ReturnStmt:
            typecheckReturnStmt(llvm::cast<ReturnStmt>(*stmt));
            break;
        case StmtKind::VarStmt:
            typecheckVarStmt(llvm::cast<VarStmt>(*stmt));
            break;
        case StmtKind::ExprStmt:
            typecheckExpr(*llvm::cast<ExprStmt>(stmt)->expr);
            break;
        case StmtKind::DeferStmt:
            typecheckExpr(*llvm::cast<DeferStmt>(stmt)->expr);
            break;
        case StmtKind::IfStmt:
            typecheckIfStmt(llvm::cast<IfStmt>(*stmt));
            break;
        case StmtKind::SwitchStmt:
            typecheckSwitchStmt(llvm::cast<SwitchStmt>(*stmt));
            break;
        case StmtKind::WhileStmt: {
            auto* whileStmt = llvm::cast<WhileStmt>(stmt);
            stmt = whileStmt->lower();
            typecheckStmt(stmt);
            break;
        }
        case StmtKind::ForStmt:
            typecheckForStmt(llvm::cast<ForStmt>(*stmt));
            break;
        case StmtKind::ForEachStmt: {
            auto* forEachStmt = llvm::cast<ForEachStmt>(stmt);
            typecheckExpr(*forEachStmt->range);
            auto nestLevel = llvm::count_if(currentControlStmts, [](auto* stmt) { return stmt->isForStmt(); });
            stmt = forEachStmt->lower(nestLevel);
            typecheckStmt(stmt);
            break;
        }
        case StmtKind::BreakStmt:
            typecheckBreakStmt(llvm::cast<BreakStmt>(*stmt));
            break;
        case StmtKind::ContinueStmt:
            typecheckContinueStmt(llvm::cast<ContinueStmt>(*stmt));
            break;
        case StmtKind::CompoundStmt:
            typecheckCompoundStmt(llvm::cast<CompoundStmt>(*stmt));
            break;
        }
    } catch (const CompileError& error) {
        error.print();
        return false;
    }

    return true;
}
