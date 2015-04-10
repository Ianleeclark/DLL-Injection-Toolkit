#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "ProcessHandler.c"

int main() {
    HANDLE hOpenProc;
    LPVOID baseAddress;

    const char* dllPath = "F:\\Programming\\RemoteInject\\bin\\Debug\\InjectableDll.dll";

    hOpenProc = GetProcessHandle("Looping.exe");
    if(hOpenProc == 0) {
        printf("Failed to find remote process id\n");
        return 0;
    }

    baseAddress = RemoteOpenAlloc(hOpenProc, dllPath);
    if(baseAddress == NULL) {
        printf("Failed to allocate memory in remote process");
        return -1;
    }

    LPDWORD baseInj;
    baseInj = Inject(hOpenProc, baseAddress, dllPath);
    CallRemoteFunction(hOpenProc,dllPath, baseInj, "testFunc");



    CleanUp(hOpenProc, baseAddress);
    return 0;
}
