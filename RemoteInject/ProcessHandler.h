#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <stdio.h>
#include <TlHelp32.h>
#include <winbase.h>

DWORD GetProcessList(const char *);
LPVOID RemoteOpenAlloc(HANDLE, const char*);
int FindBaseAddress();

#endif // PROCESSHANDLER_H
