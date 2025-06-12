#pragma once

#include <cassert>
#include <iosfwd>
#include <string>
#include <utility> // std::move
#include <vector>
#pragma warning(push, 0)
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Support/raw_ostream.h>
#pragma warning(pop)
#include "../ast/location.h"

namespace llvm {
class StringRef;
}

#ifndef NDEBUG
#define ASSERT(condition, ...) assert(condition)
#else
// Prevent unused variable warnings without evaluating the condition values.
#define ASSERT(condition, ...) ((void)sizeof(condition))
#endif

namespace cx {

std::ostream& operator<<(std::ostream& stream, llvm::StringRef string);

template<typename SourceContainer, typename Mapper> auto map(const SourceContainer& source, Mapper mapper) -> std::vector<decltype(mapper(*source.begin()))> {
    std::vector<decltype(mapper(*source.begin()))> result;
    result.reserve(source.size());
    for (auto& element : source) {
        result.emplace_back(mapper(element));
    }
    return result;
}

#define NOTNULL(x) (ASSERT(x), x)

struct StringFormatter : llvm::raw_string_ostream {
    StringFormatter() : llvm::raw_string_ostream(message) {}

private:
    std::string message;
};

std::string readLineFromFile(Location location);
void renameFile(llvm::Twine sourcePath, llvm::Twine targetPath);
void printDiagnostic(Location location, llvm::StringRef type, llvm::raw_ostream::Colors color, llvm::StringRef message);

struct Note {
    Location location;
    std::string message;
};

struct CompileError : std::exception {
    CompileError(Location location, std::string&& message, std::vector<Note>&& notes = {});
    /// Creates a specific type of compile error used for propagating non-reported errors that are the result of a previous, reported error.
    /// For example using a variable whose initializer has an error.
    static CompileError dependentError() { return CompileError(Location(), ""); }
    const char* what() const noexcept override { return message.c_str(); }
    void report() const;
    void reportAsWarning() const;

    Location location;
    std::string message; // Empty if this is a silent "dependent error", resulting from a previous, reported error.
    std::vector<Note> notes;
};

template<typename T> void printColored(const T& text, llvm::raw_ostream::Colors color) {
    if (llvm::outs().has_colors()) llvm::outs().changeColor(color, true);
    llvm::outs() << text;
    if (llvm::outs().has_colors()) llvm::outs().resetColor();
}

void printStackTrace();
[[noreturn]] void abort(StringFormatter& message);
void reportError(Location location, StringFormatter& message, llvm::ArrayRef<Note> notes = {});
void reportWarning(Location location, StringFormatter& message, llvm::ArrayRef<Note> notes = {});

#define ABORT(args) \
    { \
        printStackTrace(); \
        StringFormatter s; \
        s << args; \
        abort(s); \
    }

#define ERROR(location, args) \
    { \
        printStackTrace(); \
        StringFormatter s; \
        s << args; \
        throw CompileError(location, std::move(s.str())); \
    }

#define ERROR_WITH_NOTES(location, notes, args) \
    { \
        printStackTrace(); \
        StringFormatter s; \
        s << args; \
        throw CompileError(location, std::move(s.str()), notes); \
    }

#define REPORT_ERROR(location, args) \
    { \
        printStackTrace(); \
        StringFormatter s; \
        s << args; \
        reportError(location, s, {}); \
    }

#define REPORT_ERROR_WITH_NOTES(location, notes, args) \
    { \
        printStackTrace(); \
        StringFormatter s; \
        s << args; \
        reportError(location, s, notes); \
    }

#define WARN(location, args) \
    { \
        StringFormatter s; \
        s << args; \
        reportWarning(location, s); \
    }

std::optional<std::string> findExternalCCompiler();

} // namespace cx
