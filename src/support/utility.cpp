#include "utility.h"
#include <algorithm>
#include <fstream>
#include <ostream>
#pragma warning(push, 0)
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/Signals.h>
#pragma warning(pop)

using namespace cx;

namespace cx {
extern int errors;
extern llvm::cl::opt<int> errorLimit;
extern llvm::cl::opt<bool> disableWarnings;
extern llvm::cl::opt<bool> warningsAsErrors;
} // namespace cx

std::ostream& cx::operator<<(std::ostream& stream, llvm::StringRef string) {
    return stream.write(string.data(), string.size());
}

std::string cx::readLineFromFile(Location location) {
    std::ifstream file(location.file);

    while (--location.line) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string lineContent;
    std::getline(file, lineContent);
    return lineContent;
}

void cx::renameFile(llvm::Twine sourcePath, llvm::Twine targetPath) {
    auto permissions = llvm::sys::fs::getPermissions(sourcePath);
    if (auto error = permissions.getError()) {
        ABORT("couldn't get permissions for '" << sourcePath << "': " << error.message());
    }
    if (auto error = llvm::sys::fs::copy_file(sourcePath, targetPath)) {
        ABORT("couldn't copy '" << sourcePath << "' to '" << targetPath << "': " << error.message());
    }
    if (auto error = llvm::sys::fs::setPermissions(targetPath, *permissions)) {
        ABORT("couldn't set permissions for '" << targetPath << "': " << error.message());
    }
    if (auto error = llvm::sys::fs::remove(sourcePath)) {
        ABORT("couldn't remove '" << sourcePath << "': " << error.message());
    }
}

void cx::printDiagnostic(Location location, llvm::StringRef type, llvm::raw_ostream::Colors color, llvm::StringRef message) {
    if (llvm::outs().has_colors()) {
        llvm::outs().changeColor(llvm::raw_ostream::SAVEDCOLOR, true);
    }

    if (location.print()) {
        llvm::outs() << ": ";
    }

    printColored(type, color);
    printColored(": ", color);
    printColored(message, llvm::raw_ostream::SAVEDCOLOR);

    if (location.file && *location.file && location.isValid()) {
        auto line = readLineFromFile(location);
        llvm::outs() << '\n' << line << '\n';

        for (char ch : line.substr(0, location.column - 1)) {
            llvm::outs() << (ch != '\t' ? ' ' : '\t');
        }
        printColored('^', llvm::raw_ostream::GREEN);
    }

    llvm::outs() << '\n';
}

CompileError::CompileError(Location location, std::string&& message, std::vector<Note>&& notes)
: location(location), message(std::move(message)), notes(std::move(notes)) {}

void CompileError::report() const {
    if (message.empty()) return;
    reportError(location, StringBuilder() << message, notes);
}

void CompileError::reportAsWarning() const {
    if (message.empty()) return;
    reportWarning(location, StringBuilder() << message, notes);
}

std::optional<std::string> cx::findExternalCCompiler() {
#ifdef _WIN32
    auto compilers = {"cl.exe", "clang-cl.exe"};
#else
    auto compilers = {"cc", "clang", "gcc"};
#endif
    for (const char* compiler : compilers) {
        if (auto path = llvm::sys::findProgramByName(compiler)) {
            return std::move(*path);
        }
    }
    return std::nullopt;
}

void cx::printStackTrace() {
    if (auto env = llvm::sys::Process::GetEnv("CX_PRINT_STACK_TRACE")) {
        if (llvm::StringRef(*env).equals_insensitive("true") || *env == "1") {
            llvm::sys::PrintStackTrace(llvm::errs());
        }
    }
}

void cx::abort(llvm::StringRef message) {
    printColored("error: ", llvm::raw_ostream::RED);
    llvm::outs() << message << '\n';
    exit(1);
}

void cx::reportError(Location location, llvm::StringRef message, llvm::ArrayRef<Note> notes) {
    errors++;
    if (errorLimit > 0 && errors > errorLimit) exit(1);

    printDiagnostic(location, "error", llvm::raw_ostream::RED, message);

    for (auto& note : notes) {
        printDiagnostic(note.location, "note", llvm::raw_ostream::BLACK, note.message);
    }
}

void cx::reportWarning(Location location, llvm::StringRef message, llvm::ArrayRef<Note> notes) {
    if (disableWarnings) return;

    if (warningsAsErrors) {
        reportError(location, message, notes);
    } else {
        printDiagnostic(location, "warning", llvm::raw_ostream::YELLOW, message);

        for (auto& note : notes) {
            printDiagnostic(note.location, "note", llvm::raw_ostream::BLACK, note.message);
        }
    }
}
