#include "c-backend.h"

using namespace cx;

void CGenerator::codegenModule(const IRModule& module) {
    stream << "\n";
    stream << "// Module '" << module.name << "' forward declarations\n";
    for (auto* function : module.functions) {
        if (module.name == "std" && llvm::sys::path::filename(function->location.file) == "libc.cx") {
            continue; // Don't emit the declarations for the C standard library functions in libc.cx, instead rely on including the actual C library headers.
        }
        codegenFunctionPrototype(function);
        stream << ";\n";
    }
    stream << "\n// Module '" << module.name << "' definitions";
    for (auto* function : module.functions) {
        if (function->isExtern) continue;
        if (alreadyDefinedFunctions.count(function->mangledName)) continue;
        alreadyDefinedFunctions.insert(function->mangledName);
        codegenFunction(function);
    }
}

void CGenerator::codegenAlloca(const AllocaInst* inst) {
    stream.indent(4);
    codegenType(stream, inst->allocatedType, true);
    stream << ' ';
    auto name = (!inst->name.empty() ? inst->name : "_alloca") + std::to_string(valueSuffixCounter++);
    stream << name;
    codegenTypeSuffix(stream, inst->allocatedType, true);
    stream << ";\n";
    emittedValues.insert({ inst, '&' + std::move(name) });
}

void CGenerator::codegenReturn(const ReturnInst* inst) {
    stream.indent(4);
    stream << "return";
    if (inst->value) {
        stream << ' ';
        codegenInst(inst->value);
    }
    stream << ";\n";
}

void CGenerator::codegenBranch(const BranchInst* inst) {
    if (inst->argument) {
        stream.indent(4) << inst->destination->parameter->name << " = ";
        codegenInst(inst->argument);
        stream << "; // branch argument\n";
    }
    stream.indent(4) << "goto " << getBlockLabel(inst->destination) << ";\n";
}

void CGenerator::codegenCondBranch(const CondBranchInst* inst) {
    if (inst->trueBlock->parameter) {
        stream.indent(4);
        codegenType(stream, inst->trueBlock->parameter->type, true);
        stream << ' ' << inst->trueBlock->parameter->name << ";\n";
    }
    if (inst->falseBlock->parameter) {
        stream.indent(4);
        codegenType(stream, inst->falseBlock->parameter->type, true);
        stream << ' ' << inst->falseBlock->parameter->name << ";\n";
    }
    stream.indent(4) << "if (";
    codegenInst(inst->condition);
    stream << ") {\n";
    if (inst->trueBlock->parameter) {
        stream.indent(8) << inst->trueBlock->parameter->name << " = ";
        codegenInst(inst->argument);
        stream << ";\n";
    }
    stream.indent(8) << "goto " << getBlockLabel(inst->trueBlock) << ";\n";
    stream.indent(4) << "} else {\n";
    if (inst->falseBlock->parameter) {
        stream.indent(8) << inst->falseBlock->parameter->name << " = ";
        codegenInst(inst->argument);
        stream << ";\n";
    }
    stream.indent(8) << "goto " << getBlockLabel(inst->falseBlock) << ";\n";
    stream.indent(4) << "}\n";
}

void CGenerator::codegenSwitch(const SwitchInst* inst) {
    stream.indent(4) << "switch (";
    codegenInst(inst->condition);
    stream << ") {\n";
    for (auto [value, block] : inst->cases) {
        stream.indent(8);
        stream << "case ";
        codegenInst(value);
        stream << ": goto " << getBlockLabel(block) << ";\n";
    }
    // TODO: handle inst->defaultBlock
    stream.indent(4) << "}\n";
}

void CGenerator::codegenLoad(const LoadInst* inst) {
    stream.indent(4);
    auto name = "_load" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = *";
    codegenInst(inst->value);
    stream << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenStore(const StoreInst* inst) {
    stream.indent(4);
    stream << "*";
    codegenInst(inst->pointer);
    stream << " = ";
    codegenInst(inst->value);
    stream << ";\n";
}

void CGenerator::codegenInsert(const InsertInst* inst) {
    stream.indent(4);
    auto name = "_insert" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = " << inst->aggregate << "; ";
    stream << inst->aggregate << "._" << inst->index << " = ";
    codegenInst(inst->value);
    stream << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenExtract(const ExtractInst* inst) {
    stream.indent(4);
    auto name = "_extract" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = ";
    stream << inst->aggregate << "._" << inst->index;
    stream << ");\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenCall(const CallInst* inst) {
    stream.indent(4);
    auto returnType = inst->function->getType()->getPointee()->getReturnType();
    std::string name;
    bool hasReturnValue = !returnType->isVoid() && !returnType->isNever();
    if (hasReturnValue) {
        name = "_call" + std::to_string(valueSuffixCounter++);
        codegenType(stream, returnType, true);
        stream << " " << name << " = ";
    }
    codegenInst(inst->function);
    stream << '(';
    for (auto& arg : inst->args) {
        codegenInst(arg);
        if (&arg != &inst->args.back()) stream << ", ";
    }
    stream << ");\n";
    if (hasReturnValue) {
        emittedValues.insert({ inst, std::move(name) });
    }
}

void CGenerator::codegenBinary(const BinaryInst* inst) {
    stream.indent(4);
    auto name = "_binop" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = ";
    codegenInst(inst->left);
    stream << ' ';
    switch (inst->op.getKind()) {
        case Token::Plus:
            stream << '+';
            break;
        case Token::Minus:
            stream << '-';
            break;
        case Token::Star:
            stream << '*';
            break;
        case Token::Slash:
            stream << '/';
            break;
        case Token::Modulo:
            stream << '%';
            break;
        case Token::LeftShift:
            stream << "<<";
            break;
        case Token::RightShift:
            stream << ">>";
            break;
        case Token::And:
            stream << '&';
            break;
        case Token::Or:
            stream << '|';
            break;
        case Token::Xor:
            stream << '^';
            break;
        case Token::Equal:
            stream << "==";
            break;
        case Token::NotEqual:
            stream << "!=";
            break;
        case Token::Less:
            stream << '<';
            break;
        case Token::LessOrEqual:
            stream << "<=";
            break;
        case Token::Greater:
            stream << '>';
            break;
        case Token::GreaterOrEqual:
            stream << ">=";
            break;
        default:
            llvm_unreachable("all cases handled");
    }
    stream << ' ';
    codegenInst(inst->right);
    stream << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenUnary(const UnaryInst* inst) {
    switch (inst->op.getKind()) {
        case Token::Plus:
            stream << '+';
            break;
        case Token::Minus:
            stream << '-';
            break;
        case Token::Not:
            stream << '!';
            break;
        case Token::Tilde:
            stream << '~';
            break;
        default:
            llvm_unreachable("all cases handled");
    }
    codegenInst(inst->operand);
}

void CGenerator::codegenGEP(const GEPInst* inst) {
    stream.indent(4);
    auto name = "_gep" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = &(";
    codegenInst(inst->pointer);
    stream << ")";
    for (auto* index : inst->indexes) {
        stream << '[';
        codegenInst(index);
        stream << ']';
    }
    stream << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenConstGEP(const ConstGEPInst* inst) {
    stream.indent(4);
    auto name = "_constgep" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = &";
    codegenInst(inst->pointer);
    stream << "->_" << inst->index << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenCast(const CastInst* inst) {
    stream.indent(4);
    auto name = "_cast" + std::to_string(valueSuffixCounter++);
    stream << "__auto_type " << name << " = ";
    stream << "(";
    codegenType(stream, inst->type, true);
    stream << ") ";
    codegenInst(inst->value);
    stream << ";\n";
    emittedValues.insert({ inst, std::move(name) });
}

void CGenerator::codegenUnreachable() {
    // Nothing for now. unreachable() is a C23 extension.
}

void CGenerator::codegenSizeof(const SizeofInst* inst) {
    stream << "sizeof(";
    codegenType(stream, inst->type, true);
    stream << ")";
}

void CGenerator::codegenBasicBlock(const BasicBlock* block) {
    if (!block->name.empty()) {
        // Extra semicolon to work around "label followed by a declaration is a C23 extension".
        stream << '\n' << getBlockLabel(block) << ": ;\n";
    }
    for (auto* inst : block->body) {
        codegenInst(inst);
    }
}

void CGenerator::codegenGlobalVariable(const GlobalVariable*) {
    // TODO: generate C code for global variables
}

void CGenerator::codegenConstantString(const ConstantString* inst) {
    stream << '"';
    stream.write_escaped(inst->value);
    stream << '"';
}

void CGenerator::codegenConstantInt(const ConstantInt* inst) {
    stream << inst->value;
}

void CGenerator::codegenConstantFP(const ConstantFP* inst) {
    write_double(stream, inst->value.convertToDouble(), {});
}

void CGenerator::codegenConstantBool(const ConstantBool* inst) {
    stream << (inst->value ? "true" : "false");
}

void CGenerator::codegenConstantNull(const ConstantNull*) {
    stream << "NULL";
}

void CGenerator::codegenUndefined(const Undefined*) {
    // TODO
}

const std::string& CGenerator::getBlockLabel(const BasicBlock* block) {
    auto it = emittedValues.find(block);
    if (it != emittedValues.end()) {
        return it->second;
    } else {
        auto name = block->name;
        std::replace(name.begin(), name.end(), '.', '_');
        llvm::raw_string_ostream os(name);
        os << valueSuffixCounter++;
        return emittedValues.insert({ block, std::move(os.str()) }).first->second;
    }
}

void CGenerator::codegenInst(const Value* value) {
    auto it = emittedValues.find(value);
    if (it != emittedValues.end()) {
        stream << it->second;
    } else {
        codegenInstImpl(value);
    }
}

void CGenerator::codegenInstImpl(const Value* value) {
    switch (value->kind) {
        case ValueKind::AllocaInst:
            return codegenAlloca(llvm::cast<AllocaInst>(value));
        case ValueKind::ReturnInst:
            return codegenReturn(llvm::cast<ReturnInst>(value));
        case ValueKind::BranchInst:
            return codegenBranch(llvm::cast<BranchInst>(value));
        case ValueKind::CondBranchInst:
            return codegenCondBranch(llvm::cast<CondBranchInst>(value));
        case ValueKind::SwitchInst:
            return codegenSwitch(llvm::cast<SwitchInst>(value));
        case ValueKind::LoadInst:
            return codegenLoad(llvm::cast<LoadInst>(value));
        case ValueKind::StoreInst:
            return codegenStore(llvm::cast<StoreInst>(value));
        case ValueKind::InsertInst:
            return codegenInsert(llvm::cast<InsertInst>(value));
        case ValueKind::ExtractInst:
            return codegenExtract(llvm::cast<ExtractInst>(value));
        case ValueKind::CallInst:
            return codegenCall(llvm::cast<CallInst>(value));
        case ValueKind::BinaryInst:
            return codegenBinary(llvm::cast<BinaryInst>(value));
        case ValueKind::UnaryInst:
            return codegenUnary(llvm::cast<UnaryInst>(value));
        case ValueKind::GEPInst:
            return codegenGEP(llvm::cast<GEPInst>(value));
        case ValueKind::ConstGEPInst:
            return codegenConstGEP(llvm::cast<ConstGEPInst>(value));
        case ValueKind::CastInst:
            return codegenCast(llvm::cast<CastInst>(value));
        case ValueKind::UnreachableInst:
            return codegenUnreachable();
        case ValueKind::SizeofInst:
            return codegenSizeof(llvm::cast<SizeofInst>(value));
        case ValueKind::BasicBlock:
            return codegenBasicBlock(llvm::cast<BasicBlock>(value));
        case ValueKind::Function:
            stream << llvm::cast<Function>(value)->mangledName;
            break;
        case ValueKind::Parameter: {
            auto* param = llvm::cast<Parameter>(value);
            stream << param->name;
            break;
        }
        case ValueKind::GlobalVariable:
            return codegenGlobalVariable(llvm::cast<GlobalVariable>(value));
        case ValueKind::ConstantString:
            return codegenConstantString(llvm::cast<ConstantString>(value));
        case ValueKind::ConstantInt:
            return codegenConstantInt(llvm::cast<ConstantInt>(value));
        case ValueKind::ConstantFP:
            return codegenConstantFP(llvm::cast<ConstantFP>(value));
        case ValueKind::ConstantBool:
            return codegenConstantBool(llvm::cast<ConstantBool>(value));
        case ValueKind::ConstantNull:
            return codegenConstantNull(llvm::cast<ConstantNull>(value));
        case ValueKind::Undefined:
            return codegenUndefined(llvm::cast<Undefined>(value));
    }
}

void CGenerator::codegenFunctionPrototype(const Function* function) {
    codegenType(stream, function->returnType, !function->isExtern);
    stream << ' ' << function->mangledName << '(';
    for (auto& param : function->params) {
        codegenType(stream, param.type, !function->isExtern);
        stream << ' ' << param.name;
        if (&param != &function->params.back()) stream << ", ";
    }
    stream << ')';
}

void CGenerator::codegenFunction(const Function* function) {
    stream << '\n';
    codegenFunctionPrototype(function);
    if (function->isExtern) {
        stream << ';';
    } else {
        stream << " {\n";
        valueSuffixCounter = 0;
        for (auto* block : function->body) {
            codegenBasicBlock(block);
        }
        stream << '}';
    }
    stream << '\n';
}

void CGenerator::codegenType(llvm::raw_string_ostream& stream, IRType* type, bool needsTypeDefinition) {
    switch (type->kind) {
        case IRTypeKind::IRBasicType: {
            auto* basicType = llvm::cast<IRBasicType>(type);
            llvm::StringRef cType = llvm::StringSwitch<llvm::StringRef>(basicType->name)
                                        .Case("void", "void")
                                        .Case("bool", "_Bool")
                                        .Case("char", "char")
                                        .Case("int", "int")
                                        .Case("int8", "int8_t")
                                        .Case("int16", "int16_t")
                                        .Case("int32", "int32_t")
                                        .Case("int64", "int64_t")
                                        .Case("uint", "unsigned")
                                        .Case("uint8", "uint8_t")
                                        .Case("uint16", "uint16_t")
                                        .Case("uint32", "uint32_t")
                                        .Case("uint64", "uint64_t")
                                        .Case("float", "float")
                                        .Case("float32", "float")
                                        .Case("float64", "double")
                                        .Case("float80", "__float80")
                                        .Default(llvm::StringRef());
            if (cType.empty()) {
                llvm_unreachable("unhandled type");
            } else {
                stream << cType;
            }
            break;
        }
        case IRTypeKind::IRPointerType:
            codegenType(stream, llvm::cast<IRPointerType>(type)->pointee, false);
            stream << '*';
            break;
        case IRTypeKind::IRFunctionType:
            // TODO
            break;
        case IRTypeKind::IRArrayType: {
            auto* arrayType = llvm::cast<IRArrayType>(type);
            codegenType(stream, arrayType->elementType, needsTypeDefinition);
            // Array size will be generated by codegenTypeSuffix.
            break;
        }
        case IRTypeKind::IRStructType: {
            auto* irStruct = llvm::cast<IRStructType>(type);
            if (irStruct->name == "never") {
                stream << "void";
            } else {
                if (needsTypeDefinition) codegenTypeDefinition(preludeStream, type);
                stream << "struct " << irStruct->mangledName;
            }
            break;
        }
        case IRTypeKind::IRUnionType:
            stream << "union " << llvm::cast<IRUnionType>(type)->name;
            break;
    }
}

void CGenerator::codegenTypeSuffix(llvm::raw_string_ostream& stream, IRType* type, bool) {
    if (type->kind == IRTypeKind::IRArrayType) {
        auto* arrayType = llvm::cast<IRArrayType>(type);
        stream << "[" << arrayType->size << "]";
    }
}

void CGenerator::codegenTypeDefinition(llvm::raw_string_ostream& stream, IRType* type) {
    // Generate struct definition if this is the first time we encounter this type.
    auto* irStruct = llvm::dyn_cast<IRStructType>(type);
    if (irStruct && alreadyEmittedTypes.count(type) == 0) {
        alreadyEmittedTypes.insert(type);

        // Generate type dependencies first.
        for (auto* elementType : irStruct->elementTypes) {
            codegenTypeDefinition(stream, elementType);
        }

        stream << "\nstruct " << irStruct->mangledName << " {\n";
        int memberIndex = 0;
        for (auto* elementType : irStruct->elementTypes) {
            stream.indent(4);
            codegenType(stream, elementType, true);
            stream << " _" << (memberIndex++) << ";\n"; // TODO: add names to IRStruct fields for clearer C code gen?
        }
        stream << "};\n";
    }
}

std::string CGenerator::finish() {
    return "#include <stdio.h>\n"
           "#include <stdint.h>\n"
           "#include <stdlib.h>\n"
           "#include <string.h>\n"
           "#include <stdbool.h>\n" +
           preludeStream.str() + stream.str();
}
