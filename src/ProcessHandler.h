#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <stdio.h>
#include <TlHelp32.h>
#include <winbase.h>
#include <psapi.h>

DWORD FindProcID(const char*);

HANDLE OpenProc(DWORD);

MODULEENTRY32 FindRemoteModule(DWORD, const char*);

LPVOID RemoteOpenAlloc(HANDLE, const char*);

int FindBaseAddress();

void CleanUp(HANDLE, LPVOID);

#endif // PROCESSHANDLER_H
