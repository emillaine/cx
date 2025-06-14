#include "c-import.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#pragma warning(push, 0)
#include <clang/AST/Decl.h>
#include <clang/AST/DeclGroup.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/Type.h>
#include <clang/Basic/Builtins.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <clang/Parse/ParseAST.h>
#include <clang/Sema/Sema.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/SaveAndRestore.h>
#include <llvm/TargetParser/Host.h>
#pragma warning(pop)
#include "../ast/decl.h"
#include "../ast/module.h"
#include "../ast/type.h"
#include "../driver/driver.h"
#include "../support/utility.h"
#include "typecheck.h"

using namespace cx;
using namespace llvm::sys;

namespace {

struct CToCxConverter final : clang::ASTConsumer {
    CToCxConverter(Module& module, Typechecker& typechecker, clang::TargetInfo* targetInfo, clang::SourceManager& sourceManager)
    : module(module), typechecker(typechecker), targetInfo(targetInfo), sourceManager(sourceManager) {}

    Type getIntTypeByWidth(unsigned widthInBits, bool asSigned) {
        switch (widthInBits) {
        case 8:
            return asSigned ? Type::getInt8() : Type::getUInt8();
        case 16:
            return asSigned ? Type::getInt16() : Type::getUInt16();
        case 32:
            return asSigned ? Type::getInt32() : Type::getUInt32();
        case 64:
            return asSigned ? Type::getInt64() : Type::getUInt64();
        }
        llvm_unreachable("unsupported integer width");
    }

    Type toCx(const clang::BuiltinType& type) {
        switch (type.getKind()) {
        case clang::BuiltinType::Void:
            return Type::getVoid();
        case clang::BuiltinType::Bool:
            return Type::getBool();
        case clang::BuiltinType::Char_S:
        case clang::BuiltinType::Char_U:
            return Type::getChar();
        case clang::BuiltinType::SChar:
            return getIntTypeByWidth(targetInfo->getCharWidth(), true);
        case clang::BuiltinType::UChar:
            return getIntTypeByWidth(targetInfo->getCharWidth(), false);
        case clang::BuiltinType::Short:
            return getIntTypeByWidth(targetInfo->getShortWidth(), true);
        case clang::BuiltinType::UShort:
            return getIntTypeByWidth(targetInfo->getShortWidth(), false);
        case clang::BuiltinType::Int:
            return Type::getInt();
        case clang::BuiltinType::UInt:
            return Type::getUInt();
        case clang::BuiltinType::Long:
            return getIntTypeByWidth(targetInfo->getLongWidth(), true);
        case clang::BuiltinType::ULong:
            return getIntTypeByWidth(targetInfo->getLongWidth(), false);
        case clang::BuiltinType::LongLong:
            return getIntTypeByWidth(targetInfo->getLongLongWidth(), true);
        case clang::BuiltinType::ULongLong:
            return getIntTypeByWidth(targetInfo->getLongLongWidth(), false);
        case clang::BuiltinType::Float16:
        case clang::BuiltinType::BFloat16:
            return Type::getFloat16();
        case clang::BuiltinType::Float:
            return Type::getFloat();
        case clang::BuiltinType::Double:
            return Type::getFloat64();
        case clang::BuiltinType::LongDouble:
            return Type::getFloat80();
        case clang::BuiltinType::Int128:
            return Type::getInt128();
        case clang::BuiltinType::UInt128:
            return Type::getUInt128();
        default:
            auto name = type.getName(clang::PrintingPolicy({}));
            WARN(Location(), "unknown C built-in type '" << name << "', defaulting to 'int'");
            return Type::getInt();
        }
    }

    static llvm::StringRef getName(const clang::TagDecl& decl) {
        if (!decl.getName().empty()) {
            return decl.getName();
        } else if (auto typedefNameDecl = decl.getTypedefNameForAnonDecl()) {
            return typedefNameDecl->getName();
        }
        return "";
    }

    Type toCx(clang::QualType qualType) {
        auto mutability = qualType.isConstQualified() ? Mutability::Const : Mutability::Mutable;
        auto& type = *qualType.getTypePtr();

        switch (type.getTypeClass()) {
        case clang::Type::Pointer: {
            auto pointeeType = llvm::cast<clang::PointerType>(type).getPointeeType();
            if (pointeeType->isFunctionType()) {
                return OptionalType::get(toCx(pointeeType), mutability);
            }
            return OptionalType::get(PointerType::get(toCx(pointeeType), Mutability::Mutable), mutability);
        }
        case clang::Type::Builtin:
            return toCx(llvm::cast<clang::BuiltinType>(type)).withMutability(mutability);
        case clang::Type::Typedef: {
            auto& typedefType = llvm::cast<clang::TypedefType>(type);
            auto desugared = typedefType.desugar();
            if (mutability == Mutability::Const) desugared.addConst();
            return toCx(desugared);
        }
        case clang::Type::Elaborated:
            return toCx(llvm::cast<clang::ElaboratedType>(type).getNamedType());
        case clang::Type::Record: {
            auto& recordType = llvm::cast<clang::RecordType>(type);
            auto* recordDecl = recordType.getDecl();
            auto cxType = BasicType::get(getName(*recordDecl), {}, mutability);
            llvm::cast<BasicType>(cxType.getBase())->setDecl(toCx(*recordDecl));
            return cxType;
        }
        case clang::Type::Paren:
            return toCx(llvm::cast<clang::ParenType>(type).getInnerType());
        case clang::Type::FunctionProto: {
            auto& functionProtoType = llvm::cast<clang::FunctionProtoType>(type);
            auto paramTypes = map(functionProtoType.getParamTypes(), [&](clang::QualType paramType) { return toCx(paramType); });
            return FunctionType::get(toCx(functionProtoType.getReturnType()), std::move(paramTypes), functionProtoType.isVariadic(), mutability);
        }
        case clang::Type::FunctionNoProto: {
            auto& functionNoProtoType = llvm::cast<clang::FunctionNoProtoType>(type);
            return FunctionType::get(toCx(functionNoProtoType.getReturnType()), {}, true, mutability);
        }
        case clang::Type::ConstantArray: {
            auto& constantArrayType = llvm::cast<clang::ConstantArrayType>(type);
            if (!constantArrayType.getSize().isIntN(64)) {
                ERROR(Location(), "array is too large");
            }
            return ArrayType::get(toCx(constantArrayType.getElementType()), constantArrayType.getSize().getLimitedValue());
        }
        case clang::Type::IncompleteArray:
            return ArrayType::get(toCx(llvm::cast<clang::IncompleteArrayType>(type).getElementType()), ArrayType::UnknownSize);
        case clang::Type::Attributed:
            return toCx(llvm::cast<clang::AttributedType>(type).getEquivalentType());
        case clang::Type::Decayed:
            return toCx(llvm::cast<clang::DecayedType>(type).getDecayedType());
        case clang::Type::Enum: {
            auto& enumType = llvm::cast<clang::EnumType>(type);
            auto name = getName(*enumType.getDecl());

            if (name.empty()) {
                return toCx(enumType.getDecl()->getIntegerType());
            } else {
                return BasicType::get(name, {}, mutability);
            }
        }
        case clang::Type::Vector: {
            auto& vectorType = llvm::cast<clang::VectorType>(type);
            return ArrayType::get(toCx(vectorType.getElementType()), vectorType.getNumElements());
        }
        default:
            WARN(Location(), "unhandled type class '" << type.getTypeClassName() << "' (importing type '" << qualType.getAsString() << "')");
            return Type::getInt();
        }
    }

    std::optional<FieldDecl> toCx(const clang::FieldDecl& decl, TypeDecl& typeDecl) {
        if (decl.getName().empty()) return std::nullopt;
        return FieldDecl(toCx(decl.getType()), decl.getNameAsString(), nullptr, typeDecl, AccessLevel::Default, Location());
    }

    TypeDecl* toCx(const clang::RecordDecl& recordDecl) {
        auto it = importedRecordDecls.find(&recordDecl);
        if (it != importedRecordDecls.end()) {
            return it->second;
        }

        auto tag = recordDecl.isUnion() ? TypeTag::Union : TypeTag::Struct;
        auto* typeDecl = new TypeDecl(tag, getName(recordDecl).str(), {}, {}, AccessLevel::Default, module, nullptr, Location());
        typeDecl->packed = recordDecl.hasAttr<clang::PackedAttr>();
        importedRecordDecls.emplace(&recordDecl, typeDecl);

        // Add to symbol table before type-checking so that type-checker finds the struct decl.
        module.addToSymbolTable(typeDecl);
        module.getSourceFiles().front().topLevelDecls.push_back(typeDecl);

        bool hasFieldWithAnonymousType = false;
        for (auto* field : recordDecl.fields()) {
            if (auto fieldDecl = toCx(*field, *typeDecl)) {
                if (fieldDecl->type.isBasicType() && fieldDecl->type.getName().empty()) {
                    hasFieldWithAnonymousType = true;
                }
                typeDecl->fields.emplace_back(std::move(*fieldDecl));
            } else {
                return nullptr;
            }
        }

        // Fields with unnameable types not yet supported.
        if (!recordDecl.getName().empty() && recordDecl.isStruct() && !hasFieldWithAnonymousType) {
            // TODO: Add types in 'addAutogeneratedConstructor' so we don't need to type-check afterwards?
            typeDecl->addAutogeneratedConstructor();
            // Add types to the autogenerated constructor for IRGen:
            llvm::SaveAndRestore setModule(typechecker.currentModule, &module);
            llvm::SaveAndRestore setSourceFile(typechecker.currentSourceFile, &module.getSourceFiles().front());
            typechecker.typecheckTypeDecl(*typeDecl);
        }
        return typeDecl;
    }

    /*
    static VarDecl* toCx(const clang::VarDecl& decl) {
        return new VarDecl(toCx(decl.getType()), decl.getName().str(), nullptr, nullptr, AccessLevel::Default, module, Location());
    }
    */

    void addIntegerConstantToSymbolTable(llvm::StringRef name, llvm::APSInt value, clang::QualType qualType) {
        auto initializer = new IntLiteralExpr(std::move(value), Location());
        auto type = toCx(qualType).withMutability(Mutability::Const);
        initializer->setType(type);
        auto* varDecl = new VarDecl(type, name.str(), initializer, nullptr, AccessLevel::Default, module, Location());
        module.addToSymbolTable(varDecl);
        module.getSourceFiles().front().topLevelDecls.push_back(varDecl);
    }

    void addFloatConstantToSymbolTable(llvm::StringRef name, llvm::APFloat value) {
        auto initializer = new FloatLiteralExpr(std::move(value), Location());
        auto type = Type::getFloat64(Mutability::Const);
        initializer->setType(type);
        auto* varDecl = new VarDecl(type, name.str(), initializer, nullptr, AccessLevel::Default, module, Location());
        module.addToSymbolTable(varDecl);
        module.getSourceFiles().front().topLevelDecls.push_back(varDecl);
    }

    bool HandleTopLevelDecl(clang::DeclGroupRef declGroup) override {
        for (clang::Decl* decl : declGroup) {
            try {
                switch (decl->getKind()) {
                case clang::Decl::Function: {
                    auto functionDecl = toCx(*llvm::cast<clang::FunctionDecl>(decl));
                    if (module.getSymbolTable().findInTopLevelScope(functionDecl->getName()).empty()) {
                        module.addToSymbolTable(functionDecl);
                        module.getSourceFiles().front().topLevelDecls.push_back(functionDecl);
                    }
                    break;
                }
                case clang::Decl::Record: {
                    if (!decl->isFirstDecl()) break;
                    toCx(llvm::cast<clang::RecordDecl>(*decl));
                    break;
                }
                case clang::Decl::Enum: {
                    auto& enumDecl = llvm::cast<clang::EnumDecl>(*decl);
                    auto type = getName(enumDecl).empty() ? enumDecl.getIntegerType() : clang::QualType(enumDecl.getTypeForDecl(), 0);
                    std::vector<EnumCase> cases;

                    for (clang::EnumConstantDecl* enumerator : enumDecl.enumerators()) {
                        auto enumeratorName = enumerator->getName();
                        auto value = enumerator->getInitVal();
                        auto valueExpr = new IntLiteralExpr(value, Location());
                        cases.push_back(EnumCase(enumeratorName.str(), valueExpr, Type(), AccessLevel::Default, Location()));
                        addIntegerConstantToSymbolTable(enumeratorName, value, type);
                    }

                    auto* cxEnumDecl = new EnumDecl(getName(enumDecl).str(), std::move(cases), AccessLevel::Default, module, nullptr, Location());
                    module.addToSymbolTable(cxEnumDecl);
                    module.getSourceFiles().front().topLevelDecls.push_back(cxEnumDecl);
                    break;
                }
                case clang::Decl::Var: {
                    // TODO: C global variable importing temporarily disabled.
                    // auto* varDecl = ::toCx(llvm::cast<clang::VarDecl>(*decl), &module);
                    // module.addToSymbolTable(varDecl);
                    // module.getSourceFiles().front().topLevelDecls.push_back(varDecl);
                    break;
                }
                case clang::Decl::Typedef: {
                    auto& typedefDecl = llvm::cast<clang::TypedefDecl>(*decl);
                    auto underlyingType = toCx(typedefDecl.getUnderlyingType());
                    if (underlyingType.isBasicType()) {
                        // HACK: This defines a type alias in a hacky way
                        llvm::cast<BasicType>(BasicType::get(typedefDecl.getName(), {}).getBase())->setName(underlyingType.getName().str());
                    } else {
                        // TODO: Import non-BasicType typedefs from C headers.
                    }
                    break;
                }
                default:
                    break;
                }

                // Can't throw exceptions through the Clang API as it has exceptions disabled,
                // so need to handle them here.
            } catch (const CompileError& error) {
                CompileError augmentedError = error;
                augmentedError.message.insert(0, "encountered an internal compiler error in C import: ");
                augmentedError.reportAsWarning();
            } catch (...) {
                WARN(Location(), "Unhandled exception in C import");
            }
        }

        return true; // continue parsing
    }

    FunctionDecl* toCx(const clang::FunctionDecl& decl) {
        auto params =
            map(decl.parameters(), [&](clang::ParmVarDecl* param) { return ParamDecl(toCx(param->getType()), param->getNameAsString(), false, Location()); });

        FunctionProto proto(decl.getName().str(), std::move(params), toCx(decl.getReturnType()), decl.isVariadic(), true);
        if (auto asmLabelAttr = decl.getAttr<clang::AsmLabelAttr>()) {
            proto.asmLabel = asmLabelAttr->getLabel().str();
        }
        return new FunctionDecl(std::move(proto), {}, AccessLevel::Default, module, toCx(decl.getLocation()));
    }

    Location toCx(clang::SourceLocation location) {
        auto presumedLocation = sourceManager.getPresumedLoc(location);
        return Location(strdup(presumedLocation.getFilename()), presumedLocation.getLine(), presumedLocation.getColumn());
    }

private:
    Module& module;
    Typechecker& typechecker;
    clang::TargetInfo* targetInfo;
    clang::SourceManager& sourceManager;
    std::unordered_map<const clang::RecordDecl*, TypeDecl*> importedRecordDecls;
};

struct MacroImporter final : clang::PPCallbacks {
    MacroImporter(Module& module, CToCxConverter& cToCxConverter, clang::CompilerInstance& compilerInstance)
    : module(module), cToCxConverter(cToCxConverter), compilerInstance(compilerInstance) {}

    void MacroDefined(const clang::Token& name, const clang::MacroDirective* macro) override {
        if (macro->getMacroInfo()->getNumTokens() != 1) return;
        auto& token = macro->getMacroInfo()->getReplacementToken(0);

        switch (token.getKind()) {
        case clang::tok::identifier:
            module.addIdentifierReplacement(name.getIdentifierInfo()->getName(), token.getIdentifierInfo()->getName());
            break;
        case clang::tok::numeric_constant:
            importMacroConstant(name.getIdentifierInfo()->getName(), token);
            break;
        default:
            break;
        }
    }

private:
    void importMacroConstant(llvm::StringRef name, const clang::Token& token) {
        auto result = compilerInstance.getSema().ActOnNumericConstant(token);
        if (!result.isUsable()) return;
        clang::Expr* parsed = result.get();

        if (auto* intLiteral = llvm::dyn_cast<clang::IntegerLiteral>(parsed)) {
            llvm::APSInt value(intLiteral->getValue(), parsed->getType()->isUnsignedIntegerType());
            cToCxConverter.addIntegerConstantToSymbolTable(name, std::move(value), parsed->getType());
        } else if (auto* floatLiteral = llvm::dyn_cast<clang::FloatingLiteral>(parsed)) {
            cToCxConverter.addFloatConstantToSymbolTable(name, floatLiteral->getValue());
        }
    }

private:
    Module& module;
    CToCxConverter& cToCxConverter;
    clang::CompilerInstance& compilerInstance;
};

// Silently drops errors in system headers.
struct ErrorIgnoringTextDiagPrinter final : clang::TextDiagnosticPrinter {
    ErrorIgnoringTextDiagPrinter(llvm::raw_ostream& os, clang::DiagnosticOptions* diags, bool ownsOutputStream = false)
    : clang::TextDiagnosticPrinter(os, diags, ownsOutputStream), srcManager(nullptr) {}

    void HandleDiagnostic(clang::DiagnosticsEngine::Level level, const clang::Diagnostic& info) override {
        auto loc = info.getLocation();
        if (loc.isValid() && srcManager->isInSystemHeader(loc)) return;
        TextDiagnosticPrinter::HandleDiagnostic(level, info);
    }

    clang::SourceManager* srcManager;
};

} // namespace

bool cx::importCHeader(SourceFile& importer, ImportDecl& importDecl, Typechecker& typechecker) {
    llvm::StringRef headerName = importDecl.target;
    auto it = Module::getAllImportedModulesMap().find(headerName);
    if (it != Module::getAllImportedModulesMap().end()) {
        importer.addImportedModule(it->second);
        return true;
    }

    clang::CompilerInstance ci;
    auto* diagClient = new ErrorIgnoringTextDiagPrinter(llvm::errs(), new clang::DiagnosticOptions());
    ci.createDiagnostics(*llvm::vfs::getRealFileSystem(), diagClient);

    auto args = map(typechecker.options.cflags, [](auto& cflag) { return cflag.c_str(); });
    args.push_back("-fgnuc-version=4.2.1"); // Enable compatibility with GCC macros in imported headers.
    clang::CompilerInvocation::CreateFromArgs(ci.getInvocation(), args, ci.getDiagnostics());

    auto pto = std::make_shared<clang::TargetOptions>();
    pto->Triple = llvm::sys::getDefaultTargetTriple();
    auto targetInfo = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), pto);
    ci.setTarget(targetInfo);

    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
    diagClient->srcManager = &ci.getSourceManager();

    llvm::SmallString<256> importerDirectory;
    fs::real_path(importer.getFilePath(), importerDirectory);
    ci.getHeaderSearchOpts().AddPath(path::parent_path(importerDirectory), clang::frontend::Quoted, false, true);

    for (llvm::StringRef includePath : typechecker.options.importSearchPaths) {
        ci.getHeaderSearchOpts().AddPath(includePath, clang::frontend::System, false, true);
        ci.getHeaderSearchOpts().AddPath(includePath, clang::frontend::System, false, false);
    }
    for (llvm::StringRef frameworkPath : typechecker.options.frameworkSearchPaths) {
        ci.getHeaderSearchOpts().AddPath(frameworkPath, clang::frontend::System, true, true);
        ci.getHeaderSearchOpts().AddPath(frameworkPath, clang::frontend::System, true, false);
    }
    for (llvm::StringRef define : typechecker.options.defines) {
        ci.getPreprocessorOpts().addMacroDef(define);
    }

    ci.createPreprocessor(clang::TU_Complete);
    auto& pp = ci.getPreprocessor();
    pp.getBuiltinInfo().initializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());

    clang::HeaderSearch& headerSearch = ci.getPreprocessor().getHeaderSearchInfo();
    auto fileEntry = headerSearch.LookupFile(headerName, {}, false, nullptr, nullptr, {}, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    if (!fileEntry) {
        std::string searchDirs;
        for (auto& searchDir : headerSearch.search_dir_range()) {
            searchDirs += '\n';
            searchDirs += searchDir.getName();
        }
        REPORT_ERROR(importDecl.location, "couldn't find C header file '" << importDecl.target << "' in the following locations:" << searchDirs);
        return false;
    }

    auto headerPath = fileEntry->getFileEntry().tryGetRealPathName();
    if (headerPath.empty()) headerPath = headerName;
    importDecl.importedHeaderPath = headerPath;

    std::string headerModuleName = headerName.str();
    llvm::replace(headerModuleName, '.', '_');
    auto module = new Module(std::move(headerModuleName));
    module->addSourceFile(SourceFile(headerPath, module));

    auto cToCxConverter = new CToCxConverter(*module, typechecker, targetInfo, ci.getSourceManager());
    ci.setASTConsumer(std::unique_ptr<CToCxConverter>(cToCxConverter));
    ci.createASTContext();
    ci.createSema(clang::TU_Complete, nullptr);
    pp.addPPCallbacks(std::make_unique<MacroImporter>(*module, *cToCxConverter, ci));

    // Treating all imported C headers as system code for now, since we have no proper way to differentiate them from normal user code.
    auto fileID = ci.getSourceManager().createFileID(*fileEntry, clang::SourceLocation(), clang::SrcMgr::C_System);
    ci.getSourceManager().setMainFileID(fileID);
    ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(), &ci.getPreprocessor());
    clang::ParseAST(ci.getPreprocessor(), &ci.getASTConsumer(), ci.getASTContext(), false, clang::TU_Complete, nullptr, /*SkipFunctionBodies*/ true);
    ci.getDiagnosticClient().EndSourceFile();
    ci.getDiagnosticClient().finish();

    if (ci.getDiagnosticClient().getNumErrors() > 0) {
        return false;
    }

    importer.addImportedModule(module);
    Module::getAllImportedModulesMap()[headerName] = module;
    return true;
}
