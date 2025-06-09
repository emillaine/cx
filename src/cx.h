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

typedef struct CxModule CxModule;

typedef struct CxFunction {
    void* ptr;
} CxFunction;

typedef struct CxCompileResult {
    int status;
} CxCompileResult;

API_EXPORT CxModule* cxCreateModule(const char* name);

API_EXPORT void cxLoadScriptFromFile(CxModule* module, const char* filePath);

/// Compiles all scripts loaded so far.
API_EXPORT CxCompileResult cxCompileModule(CxModule* module);

API_EXPORT CxFunction cxGetFunction(CxModule* module, const char* name);

#ifdef __cplusplus
}
#endif
#endif
