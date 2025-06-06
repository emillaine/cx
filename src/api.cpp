#include "cx.h"

#ifdef _WIN32
// TODO
#else
#include <dlfcn.h>
#endif

#include "ast/mangle.h"
#include "ast/module.h"
#include "driver/driver.h"
#include "parser/parse.h"

using namespace cx;

extern "C" {

struct cxModule {
    Module module;
};

cxModule* cxCreateModule(const char* name) {
    return new cxModule { .module = Module(name) };
}

void cxLoadScript(cxModule* module, const char* script, int length) {
    module->module.fileBuffers.push_back(llvm::MemoryBuffer::getMemBuffer({ script, static_cast<size_t>(length) }, "", /*RequiresNullTerminator*/ true));
}

void cxCompileModule(cxModule* module) {
    buildModule(module->module, { .createSharedLib = true });
}

cxFunction cxGetFunction(cxModule* module, const char* name) {
    cxFunction function = {};
    auto* decl = module->module.getSymbolTable().findOne(name);

    if (auto* functionDecl = llvm::dyn_cast_or_null<FunctionDecl>(decl)) {
        auto mangledName = mangleFunctionDecl(*functionDecl);

#ifdef _WIN32
        // TODO
#else
        void* lib = dlopen("main.so", RTLD_LAZY);
        function.ptr = dlsym(lib, mangledName.c_str());
#endif
    }

    return function;
}

} // extern "C"
