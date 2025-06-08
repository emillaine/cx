#include "cx.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    cxModule* module = cxCreateModule("main");
    cxLoadScriptFromFile(module, "script1.cx");
    cxLoadScriptFromFile(module, "script2.cx");
    cxCompileModule(module);

    cxFunction function = cxGetFunction(module, "hello");
    if (function.ptr == NULL) {
        fprintf(stderr, "Function not found\n");
        return 1;
    }

    auto hello = reinterpret_cast<void (*)()>(function.ptr);
    hello();
}
