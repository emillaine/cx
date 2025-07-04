#include "typecheck.h"
#pragma warning(push, 0)
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/SaveAndRestore.h>
#pragma warning(pop)
#include "../ast/module.h"
#include "../driver/driver.h"
#include "../package-manager/manifest.h"
#include "../parser/parse.h"

using namespace cx;

TypeDecl* Typechecker::getTypeDecl(const BasicType& type) {
    if (auto* typeDecl = type.getDecl()) {
        return typeDecl;
    }

    auto decls = findDecls(type.getQualifiedName());

    if (!decls.empty()) {
        ASSERT(decls.size() == 1);
        return llvm::dyn_cast_or_null<TypeDecl>(decls[0]);
    }

    decls = findDecls(type.getName());
    if (decls.empty()) return nullptr;
    ASSERT(decls.size() == 1);
    auto instantiation = llvm::cast<TypeTemplate>(decls[0])->instantiate(type.getGenericArgs());
    getCurrentModule()->addToSymbolTable(*instantiation);
    deferTypechecking(instantiation);
    return instantiation;
}

static std::error_code importModuleSourcesInDirectoryRecursively(const llvm::Twine& directoryPath, Module& module, const CompileOptions& options) {
    std::error_code error;
    std::vector<std::string> paths;

    for (llvm::sys::fs::recursive_directory_iterator it(directoryPath, error), end; it != end; it.increment(error)) {
        if (error) break;
        if (llvm::sys::path::extension(it->path()) == ".cx") {
            paths.push_back(it->path());
        }
    }

    if (!error) {
        llvm::sort(paths);

        for (auto& path : paths) {
            Parser parser(addFileBufferToModule(path, module), module, options);
            parser.parse();
        }
    }

    if (module.getSourceFiles().empty()) {
        REPORT_ERROR(Location(), "Module '" << module.getName() << "' import failed: no source files found in '" << directoryPath << "' or its subdirectories");
    }

    return error;
}

llvm::ErrorOr<const Module&> Typechecker::importModule(SourceFile* importer, const PackageManifest* manifest, llvm::StringRef moduleName) {
    auto it = Module::getAllImportedModulesMap().find(moduleName);
    if (it != Module::getAllImportedModulesMap().end()) {
        if (importer) importer->addImportedModule(it->second);
        return *it->second;
    }

    auto module = new Module(moduleName.str());
    std::error_code error;

    if (manifest) {
        for (auto& dependency : manifest->getDeclaredDependencies()) {
            if (dependency.getPackageIdentifier() == moduleName) {
                error = importModuleSourcesInDirectoryRecursively(dependency.getFileSystemPath(), *module, options);
                goto done;
            }
        }
    }

    for (llvm::StringRef importPath : options.importSearchPaths) {
        auto modulePath = (importPath + "/" + moduleName).str();
        if (llvm::sys::fs::is_directory(modulePath)) {
            error = importModuleSourcesInDirectoryRecursively(modulePath, *module, options);
            goto done;
        }
    }

done:
    if (error) return error;
    if (importer) importer->addImportedModule(module);
    Module::getAllImportedModulesMap()[module->getName()] = module;
    typecheckModule(*module, nullptr);
    return *module;
}

void Typechecker::deferTypechecking(Decl* decl) {
    for (auto existing : declsToTypecheck) {
        if (existing == decl) {
            return;
        }
    }
    declsToTypecheck.push_back(decl);
}

void Typechecker::postProcess() {
    llvm::SaveAndRestore setPostProcessing(isPostProcessing, true);

    while (!declsToTypecheck.empty()) {
        auto currentDeclsToTypecheck = std::move(declsToTypecheck);

        for (auto* decl : currentDeclsToTypecheck) {
            switch (decl->kind) {
            case DeclKind::FunctionDecl:
            case DeclKind::MethodDecl:
            case DeclKind::ConstructorDecl:
            case DeclKind::DestructorDecl:
                typecheckFunctionDecl(*llvm::cast<FunctionDecl>(decl));
                break;
            case DeclKind::FunctionTemplate:
                typecheckFunctionTemplate(*llvm::cast<FunctionTemplate>(decl));
                break;
            case DeclKind::TypeDecl:
                typecheckTypeDecl(*llvm::cast<TypeDecl>(decl));
                break;
            default:
                llvm_unreachable("invalid deferred decl");
            }
        }
    }
}

static void checkUnusedDecls(const Module& module) {
    for (auto& sourceFile : module.getSourceFiles()) {
        for (auto& decl : sourceFile.getTopLevelDecls()) {
            if (decl->isReferenced()) continue;

            if (decl->isFunctionDecl() || decl->isFunctionTemplate()) {
                if (decl->isMain()) continue;
                WARN(decl->getLocation(), "unused declaration '" << decl->getName() << "'");
            }
        }
    }
}

void Typechecker::typecheckModule(Module& module, const PackageManifest* manifest) {
    llvm::SaveAndRestore restoreModule(currentModule);
    llvm::SaveAndRestore restoreSourceFile(currentSourceFile);

    auto stdModule = importModule(nullptr, nullptr, "std");
    if (!stdModule) {
        ABORT("couldn't import the standard library: " << stdModule.getError().message());
    }

    // Typecheck implemented interfaces so that inherited methods and fields are added to the implementing type before they're referenced.
    for (auto& sourceFile : module.getSourceFiles()) {
        for (auto& decl : sourceFile.getTopLevelDecls()) {
            currentModule = &module;
            currentSourceFile = &sourceFile;

            if (auto typeDecl = llvm::dyn_cast<TypeDecl>(decl)) {
                llvm::StringMap<Type> genericArgs = {{"This", typeDecl->getType()}};

                for (Type interface : typeDecl->interfaces) {
                    try {
                        typecheckType(interface, typeDecl->accessLevel);
                    } catch (const CompileError& error) {
                        error.report();
                    }
                    std::vector<FieldDecl> inheritedFields;

                    for (auto& field : interface.getDecl()->fields) {
                        inheritedFields.push_back(field.instantiate(genericArgs, *typeDecl));
                    }

                    typeDecl->fields.insert(typeDecl->fields.begin(), inheritedFields.begin(), inheritedFields.end());

                    for (auto member : interface.getDecl()->methods) {
                        auto methodDecl = llvm::cast<MethodDecl>(member);
                        if (methodDecl->body) {
                            auto copy = methodDecl->instantiate(genericArgs, {}, *typeDecl);
                            getCurrentModule()->addToSymbolTable(*copy);
                            typeDecl->addMethod(copy);
                        }
                    }
                }
            }
        }
    }

    // Infer the types of global variables for use before their declaration.
    for (auto& sourceFile : module.getSourceFiles()) {
        currentModule = &module;
        currentSourceFile = &sourceFile;

        for (auto& decl : sourceFile.getTopLevelDecls()) {
            if (auto* varDecl = llvm::dyn_cast<VarDecl>(decl)) {
                try {
                    typecheckVarDecl(*varDecl);
                } catch (const CompileError& error) {
                    error.report();
                }
            }
        }

        postProcess();
    }

    for (auto& sourceFile : module.getSourceFiles()) {
        for (auto& decl : sourceFile.getTopLevelDecls()) {
            currentModule = &module;
            currentSourceFile = &sourceFile;

            if (!decl->isVarDecl()) {
                try {
                    typecheckTopLevelDecl(*decl, manifest);
                    postProcess();
                } catch (const CompileError& error) {
                    error.report();
                }
            }
        }
    }

    if (module.getName() != "std" && !options.noUnusedWarnings) {
        checkUnusedDecls(module);
    }
}

static llvm::SmallVector<Decl*, 8> findDeclsInModules(llvm::StringRef name, llvm::ArrayRef<Module*> modules, bool topLevelOnly = false) {
    ASSERT(!name.empty());
    llvm::SmallVector<Decl*, 8> decls;

    for (auto& module : modules) {
        auto matches = topLevelOnly ? module->symbolTable.findInTopLevelScope(name) : module->symbolTable.findFirst(name);
        llvm::append_range(decls, matches);
    }

    return decls;
}

static Decl* findDeclInModules(llvm::StringRef name, Location location, llvm::ArrayRef<Module*> modules) {
    ASSERT(!name.empty());
    auto decls = findDeclsInModules(name, modules);

    if (decls.size() == 1) {
        return decls[0];
    } else if (decls.empty()) {
        return nullptr;
    } else if (llvm::all_of(decls, [](Decl* decl) { return decl->getModule() && decl->getModule()->name.ends_with("_h"); })) {
        // For duplicate definitions in C headers, return the last definition.
        // TODO: This should only work for declarations of the same thing.
        return decls.back(); // For duplicate definitions in C headers, return the last definition.
    } else {
        ERROR(location, "ambiguous reference to '" << name << "'");
    }
}

Decl* Typechecker::findDecl(llvm::StringRef name, Location location) const {
    ASSERT(!name.empty());

    if (Decl* match = findDeclInModules(name, location, currentModule)) {
        return match;
    }

    if (currentFunction) {
        if (auto* typeDecl = currentFunction->getTypeDecl()) {
            for (auto& field : typeDecl->fields) {
                if (field.getName() == name) {
                    return &field;
                }
            }
        }
    }

    if (Decl* match = findDeclInModules(name, location, Module::getStdlibModule())) {
        return match;
    }

    if (Decl* match = findDeclInModules(name, location, currentSourceFile->getImportedModules())) {
        return match;
    }

    ERROR(location, "unknown identifier '" << name << "'");
}

static void appendUnique(std::vector<Decl*>& target, llvm::ArrayRef<Decl*> source) {
    for (auto& element : source) {
        // TODO: Should this ever be false? I.e. should the same decl ever be in multiple different modules?
        if (!llvm::is_contained(target, element)) {
            target.push_back(element);
        }
    }
}

std::vector<Decl*> Typechecker::findDecls(llvm::StringRef name, TypeDecl* receiverTypeDecl, bool inAllImportedModules) const {
    ASSERT(!name.empty());
    std::vector<Decl*> decls;

    if (!receiverTypeDecl && currentFunction) {
        receiverTypeDecl = currentFunction->getTypeDecl();
    }

    if (receiverTypeDecl) {
        for (auto& decl : receiverTypeDecl->methods) {
            if (auto* functionDecl = llvm::dyn_cast<FunctionDecl>(decl)) {
                if (functionDecl->getName() == name) {
                    decls.emplace_back(decl);
                }
            } else if (auto* functionTemplate = llvm::dyn_cast<FunctionTemplate>(decl)) {
                if (functionTemplate->getQualifiedName() == name) {
                    decls.emplace_back(decl);
                }
            }
        }

        for (auto& field : receiverTypeDecl->fields) {
            // TODO: Only one comparison should be needed.
            if (field.getName() == name || field.getQualifiedName() == name) {
                decls.emplace_back(&field);
            }
        }
    }

    if (currentModule->getName() != "std") {
        appendUnique(decls, findDeclsInModules(name, currentModule, false));
        appendUnique(decls, findDeclsInModules(name, currentModule, true)); // HACK, TODO: one find function should be enough
    }

    appendUnique(decls, findDeclsInModules(name, Module::getStdlibModule()));

    if (currentSourceFile && !inAllImportedModules) {
        appendUnique(decls, findDeclsInModules(name, currentSourceFile->getImportedModules()));
    } else {
        appendUnique(decls, findDeclsInModules(name, Module::getAllImportedModules()));
    }

    return decls;
}
