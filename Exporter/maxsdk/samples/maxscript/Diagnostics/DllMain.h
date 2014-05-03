#pragma once

#include <WindowsDefines.h>
#include <strbasic.h>
extern HINSTANCE hInstance;


BOOL WINAPI DllMain(HINSTANCE DLLhinst, DWORD fdwReason, LPVOID lpvReserved);
// ========================================================
__declspec(dllexport) void LibInit();

__declspec(dllexport) const MCHAR* LibDescription();

__declspec(dllexport) ULONG LibVersion();