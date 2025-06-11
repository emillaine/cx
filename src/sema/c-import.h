#pragma once

namespace llvm {
class StringRef;
} // namespace llvm

namespace cx {

struct ImportDecl;
struct SourceFile;
struct Location;
struct CompileOptions;
struct Typechecker;

/// Returns true if the header was found and successfully imported.
bool importCHeader(SourceFile& importer, ImportDecl& importDecl, Typechecker& typechecker);

} // namespace cx
