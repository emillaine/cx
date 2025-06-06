#pragma once

#include "../ast/token.h"

namespace llvm {
class MemoryBuffer;
}

namespace cx {

struct SourceLocation;

struct Lexer {
    Lexer(llvm::MemoryBufferRef input);
    Token nextToken();
    const char* getFilePath() const;

private:
    SourceLocation getCurrentLocation() const;
    char readChar();
    void unreadChar(char ch);
    void readBlockComment(SourceLocation startLocation);
    Token readQuotedLiteral(char delimiter, Token::Kind literalKind);
    Token readNumber();

    llvm::MemoryBufferRef buffer;
    const char* currentFilePosition;
    SourceLocation firstLocation;
    SourceLocation lastLocation;
};

} // namespace cx
