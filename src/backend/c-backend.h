#pragma once

#include "ir.h"
#include <unordered_set>

namespace cx {

struct CGenerator {
    CGenerator() : preludeStream(prelude), stream(result) {}
    void codegenModule(const IRModule& module);
    void codegenAlloca(const AllocaInst* inst);
    void codegenReturn(const ReturnInst* inst);
    void codegenBranch(const BranchInst* inst);
    void codegenCondBranch(const CondBranchInst* inst);
    void codegenSwitch(const SwitchInst* inst);
    void codegenLoad(const LoadInst* inst);
    void codegenStore(const StoreInst* inst);
    void codegenInsert(const InsertInst* inst);
    void codegenExtract(const ExtractInst* inst);
    void codegenCall(const CallInst* inst);
    void codegenBinary(const BinaryInst* inst);
    void codegenUnary(const UnaryInst* inst);
    void codegenGEP(const GEPInst* inst);
    void codegenConstGEP(const ConstGEPInst* inst);
    void codegenCast(const CastInst* inst);
    void codegenUnreachable();
    void codegenSizeof(const SizeofInst* inst);
    void codegenBasicBlock(const BasicBlock* block);
    void codegenGlobalVariable(const GlobalVariable* inst);
    void codegenConstantString(const ConstantString* inst);
    void codegenConstantInt(const ConstantInt* inst);
    void codegenConstantFP(const ConstantFP* inst);
    void codegenConstantBool(const ConstantBool* inst);
    void codegenConstantNull(const ConstantNull* inst);
    void codegenUndefined(const Undefined* inst);
    void codegenInst(const Value* value);
    void codegenInstImpl(const Value* value);
    void codegenFunctionPrototype(const Function* function);
    void codegenFunction(const Function* function);
    void codegenType(llvm::raw_string_ostream& stream, IRType* type, bool needsTypeDefinition);
    void codegenTypeSuffix(llvm::raw_string_ostream& stream, IRType* type, bool needsTypeDefinition);
    void codegenTypeDefinition(llvm::raw_string_ostream& stream, IRType* type);
    const std::string& getBlockLabel(const BasicBlock* block);
    std::string finish();

    std::string prelude;
    std::string result;
    llvm::raw_string_ostream preludeStream; // Contains struct definitions
    llvm::raw_string_ostream stream; // Contains functions
    std::unordered_set<IRType*> alreadyEmittedTypes;
    std::unordered_set<std::string> alreadyDefinedFunctions;
    std::unordered_map<const Value*, std::string> emittedValues;
    int valueSuffixCounter = 0;
};

} // namespace cx
