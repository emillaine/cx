#pragma once

#include <string>

namespace cx {

struct FunctionDecl;
struct Type;

void mangleType(llvm::raw_string_ostream& stream, Type type);
std::string mangleType(Type type);
std::string mangleFunctionDecl(const FunctionDecl& functionDecl);

} // namespace cx
