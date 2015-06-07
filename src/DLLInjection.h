#ifndef DLLINJECTION_H
#define DLLINJECTION_H

#include <stdio.h>
#include <winbase.h>

LPDWORD Inject(HANDLE, LPVOID, const char*);

LPVOID RemoteFunctionExport(const char*, HMODULE, LPCSTR);

BOOL CallRemoteFunction(HANDLE, const char*, LPVOID, const char*);

#endif // DLLINJECTION_H