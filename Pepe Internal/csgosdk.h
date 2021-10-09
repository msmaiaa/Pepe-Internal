#pragma once
#include <windows.h>

typedef void* (__cdecl* tCreateInterface)(const char* name, int* returnCode);

void* GetInterface(const wchar_t* dllname, const char* interfacename) {
    tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress(GetModuleHandle(dllname), "CreateInterface");
    int returnCode = 0;
    void* itf = CreateInterface(interfacename, &returnCode);

    return itf;
}