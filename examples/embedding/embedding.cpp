#include "cx.h"
#include <stdio.h>
#include <stdlib.h>

struct Buffer {
    char* data;
    long size;
};

Buffer readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", path);
        return {};
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(size + 1);
    if (data) {
        fread(data, 1, size, file);
        data[size] = '\0';
    } else {
        size = 0;
    }

    fclose(file);
    return {data, size};
}

int main() {
    Buffer script1 = readFile("script1.cx");
    Buffer script2 = readFile("script2.cx");
    if (script1.data == NULL || script2.data == NULL) {
        return 1;
    }

    cxModule* module = cxCreateModule("main");
    cxLoadScript(module, script1.data, script1.size);
    cxLoadScript(module, script2.data, script2.size);
    cxCompileModule(module);

    cxFunction function = cxGetFunction(module, "hello");
    if (function.ptr == NULL) {
        fprintf(stderr, "Function not found\n");
        return 1;
    }

    auto hello = reinterpret_cast<void (*)()>(function.ptr);
    hello();
}
