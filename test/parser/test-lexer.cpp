#include "parser/lex.h"
#include <cassert>
#include <cstring>
#pragma warning(push, 0)
#include <llvm/Support/MemoryBuffer.h>
#pragma warning(pop)

using namespace cx;

// The lexer must never read past the NUL terminator, even when the file has
// no trailing newline: the tail here is hostile bytes that would lex as tokens.
static void checkStickyEof() {
    char storage[64];
    memset(storage, '!', sizeof(storage));
    memcpy(storage, "f", 1);
    storage[1] = '\0';
    auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(storage, 1), "test", false);
    Lexer lexer(buffer->getMemBufferRef());
    Token first = lexer.nextToken();
    assert(first.is(Token::Identifier) && first.getString() == "f");
    for (int i = 0; i < 8; i++) {
        assert(lexer.nextToken().is(Token::None));
    }
}

static void checkBomSkipped() {
    auto buffer = llvm::MemoryBuffer::getMemBuffer("\xEF\xBB\xBFx", "test");
    Lexer lexer(buffer->getMemBufferRef());
    Token first = lexer.nextToken();
    assert(first.is(Token::Identifier) && first.getString() == "x");
    assert(first.location.line == 1 && first.location.column == 1);
    assert(lexer.nextToken().is(Token::None));
}

static void checkDegenerateInputs() {
    // Empty file: EOF immediately, and stays there.
    auto empty = llvm::MemoryBuffer::getMemBuffer("", "test");
    Lexer emptyLexer(empty->getMemBufferRef());
    assert(emptyLexer.nextToken().is(Token::None));
    assert(emptyLexer.nextToken().is(Token::None));
    // BOM-only file: skipping the BOM lands exactly on EOF.
    auto bomOnly = llvm::MemoryBuffer::getMemBuffer("\xEF\xBB\xBF", "test");
    Lexer bomLexer(bomOnly->getMemBufferRef());
    assert(bomLexer.nextToken().is(Token::None));
    assert(bomLexer.nextToken().is(Token::None));
}

int main() {
    checkStickyEof();
    checkBomSkipped();
    checkDegenerateInputs();
}
