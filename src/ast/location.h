#pragma once

#include <limits>
#pragma warning(push, 0)
#include <llvm/Support/raw_ostream.h>
#pragma warning(pop)

namespace cx {

// A location in source code.
struct Location {
    using IntegerType = short;

    const char* file;
    IntegerType line;
    IntegerType column;

    Location() : Location(nullptr, std::numeric_limits<IntegerType>::min(), std::numeric_limits<IntegerType>::min()) {}
    Location(const char* file, IntegerType line, IntegerType column) : file(file), line(line), column(column) {}
    Location nextColumn() const { return Location(file, line, column + 1); }
    bool isValid() const { return line > 0 && column > 0; }

    bool print() const {
        if (file && *file) {
            llvm::outs() << file;
            if (isValid()) {
                llvm::outs() << ':' << line << ':' << column;
            }
            return true;
        }
        return false;
    }
};

} // namespace cx
