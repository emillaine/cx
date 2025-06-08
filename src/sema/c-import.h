#pragma once

namespace llvm {
class StringRef;
} // namespace llvm

namespace cx {

struct SourceFile;
struct Location;
struct CompileOptions;

/// Returns true if the header was found and successfully imported.
bool importCHeader(SourceFile& importer, llvm::StringRef headerName, const CompileOptions& options, Location importLocation);

} // namespace cx
