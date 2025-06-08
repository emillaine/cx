#pragma once
#include "expr.h"
#include <vector>

namespace cx {

struct VarDecl;

enum class StmtKind {
    ReturnStmt,
    VarStmt,
    ExprStmt,
    DeferStmt,
    IfStmt,
    SwitchStmt,
    WhileStmt,
    ForStmt,
    ForEachStmt,
    BreakStmt,
    ContinueStmt,
    CompoundStmt,
};

struct Stmt {
    virtual ~Stmt() = 0;
    bool isReturnStmt() const { return kind == StmtKind::ReturnStmt; }
    bool isVarStmt() const { return kind == StmtKind::VarStmt; }
    bool isExprStmt() const { return kind == StmtKind::ExprStmt; }
    bool isDeferStmt() const { return kind == StmtKind::DeferStmt; }
    bool isIfStmt() const { return kind == StmtKind::IfStmt; }
    bool isSwitchStmt() const { return kind == StmtKind::SwitchStmt; }
    bool isWhileStmt() const { return kind == StmtKind::WhileStmt; }
    bool isForStmt() const { return kind == StmtKind::ForStmt; }
    bool isForEachStmt() const { return kind == StmtKind::ForEachStmt; }
    bool isBreakStmt() const { return kind == StmtKind::BreakStmt; }
    bool isContinueStmt() const { return kind == StmtKind::ContinueStmt; }
    bool isCompoundStmt() const { return kind == StmtKind::CompoundStmt; }
    bool isBreakable() const;
    bool isContinuable() const;
    Stmt* instantiate(const llvm::StringMap<Type>& genericArgs) const;

    const StmtKind kind;

protected:
    Stmt(StmtKind kind) : kind(kind) {}
};

inline Stmt::~Stmt() {}

struct ReturnStmt : Stmt {
    ReturnStmt(Expr* value, SourceLocation location) : Stmt(StmtKind::ReturnStmt), value(value), location(location) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::ReturnStmt; }

    Expr* value;
    SourceLocation location;
};

struct VarStmt : Stmt {
    VarStmt(VarDecl* decl) : Stmt(StmtKind::VarStmt), decl(decl) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::VarStmt; }

    VarDecl* decl;
};

/// A statement that consists of the evaluation of a single expression.
struct ExprStmt : Stmt {
    ExprStmt(Expr* expr) : Stmt(StmtKind::ExprStmt), expr(expr) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::ExprStmt; }

    Expr* expr;
};

struct DeferStmt : Stmt {
    DeferStmt(Expr* expr) : Stmt(StmtKind::DeferStmt), expr(expr) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::DeferStmt; }

    Expr* expr;
};

struct IfStmt : Stmt {
    IfStmt(Expr* condition, std::vector<Stmt*>&& thenBody, std::vector<Stmt*>&& elseBody)
    : Stmt(StmtKind::IfStmt), condition(condition), thenBody(std::move(thenBody)), elseBody(std::move(elseBody)) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::IfStmt; }

    Expr* condition;
    std::vector<Stmt*> thenBody;
    std::vector<Stmt*> elseBody;
};

struct SwitchCase {
    SwitchCase(Expr* value, VarDecl* associatedValue, std::vector<Stmt*>&& stmts) : value(value), associatedValue(associatedValue), stmts(std::move(stmts)) {}

    Expr* value;
    VarDecl* associatedValue;
    std::vector<Stmt*> stmts;
};

struct SwitchStmt : Stmt {
    SwitchStmt(Expr* condition, std::vector<SwitchCase>&& cases, std::vector<Stmt*>&& defaultStmts)
    : Stmt(StmtKind::SwitchStmt), condition(condition), cases(std::move(cases)), defaultStmts(std::move(defaultStmts)) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::SwitchStmt; }

    Expr* condition;
    std::vector<SwitchCase> cases;
    std::vector<Stmt*> defaultStmts;
};

struct WhileStmt : Stmt {
    WhileStmt(Expr* condition, std::vector<Stmt*>&& body, SourceLocation location)
    : Stmt(StmtKind::WhileStmt), condition(condition), body(std::move(body)), location(location) {}
    Stmt* lower();
    static bool classof(const Stmt* s) { return s->kind == StmtKind::WhileStmt; }

    Expr* condition;
    std::vector<Stmt*> body;
    SourceLocation location;
};

struct ForStmt : Stmt {
    ForStmt(VarStmt* variable, Expr* condition, Expr* increment, std::vector<Stmt*>&& body, SourceLocation location)
    : Stmt(StmtKind::ForStmt), variable(variable), condition(condition), increment(increment), body(std::move(body)), location(location) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::ForStmt; }

    VarStmt* variable;
    Expr* condition;
    Expr* increment;
    std::vector<Stmt*> body;
    SourceLocation location;
};

struct ForEachStmt : Stmt {
    ForEachStmt(VarDecl* variable, Expr* range, std::vector<Stmt*>&& body, SourceLocation location)
    : Stmt(StmtKind::ForEachStmt), variable(variable), range(range), body(std::move(body)), location(location) {}
    Stmt* lower(int nestLevel);
    static bool classof(const Stmt* s) { return s->kind == StmtKind::ForEachStmt; }

    VarDecl* variable;
    Expr* range;
    std::vector<Stmt*> body;
    SourceLocation location;
};

struct BreakStmt : Stmt {
    BreakStmt(SourceLocation location) : Stmt(StmtKind::BreakStmt), location(location) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::BreakStmt; }

    SourceLocation location;
};

struct ContinueStmt : Stmt {
    ContinueStmt(SourceLocation location) : Stmt(StmtKind::ContinueStmt), location(location) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::ContinueStmt; }

    SourceLocation location;
};

struct CompoundStmt : Stmt {
    CompoundStmt(std::vector<Stmt*>&& body) : Stmt(StmtKind::CompoundStmt), body(std::move(body)) {}
    static bool classof(const Stmt* s) { return s->kind == StmtKind::CompoundStmt; }

    std::vector<Stmt*> body;
};

} // namespace cx
