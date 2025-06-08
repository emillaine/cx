#pragma once
#ifndef CX_H
#define CX_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

typedef struct cxModule cxModule;

typedef struct cxFunction {
    void* ptr;
} cxFunction;

API_EXPORT cxModule* cxCreateModule(const char* name);

API_EXPORT void cxLoadScriptFromFile(cxModule* module, const char* filePath);

/// Compiles all scripts loaded so far.
API_EXPORT void cxCompileModule(cxModule* module);

API_EXPORT cxFunction cxGetFunction(cxModule* module, const char* name);

#ifdef __cplusplus
}
#endif
#endif
