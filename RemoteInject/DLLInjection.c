#include "DLLInjection.h"

LPDWORD Inject(HANDLE hOpenProc, LPVOID baseAddress, const char* dllPath) {
    LPVOID loadLibA;
    HANDLE threadID;
    HMODULE baseInject;
    int bytesWritten;

    loadLibA = (LPVOID)GetProcAddress(
                       GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");

    if(loadLibA == NULL) {
        printf("Failed to locate LoadLibraryA\n");
        return FALSE;
    }

    bytesWritten = WriteProcessMemory(hOpenProc, baseAddress,
                                      dllPath, strlen(dllPath) + 1, NULL);

    if(bytesWritten == 0) {
        return FALSE;
    }

    threadID = CreateRemoteThread(hOpenProc, NULL, 0, 
                                 (LPTHREAD_START_ROUTINE)loadLibA,
                                 baseAddress, 0, NULL);
    if(threadID == NULL) {
        printf("Failed to CreateRemoteThread()\n");
        return FALSE;
    } else {
        printf("Injected successfully into remote process address space\n");

        if(threadID != 0) {
            WaitForSingleObject(threadID, INFINITE);
            GetExitCodeThread(threadID, (LPDWORD)&baseInject);
            CloseHandle(threadID);
        }
        return NULL;
    }
    return NULL;
}

void* RemoteFunctionExport(const char* dllPath, HMODULE injBaseAddr,
                           LPCSTR lpFunctionName) {
    HMODULE dllLoaded;
    DWORD offset;
    void* remoteFunc;

    dllLoaded = LoadLibrary(dllPath);
    if(dllLoaded == NULL) {
        return NULL;
    } else {
    remoteFunc = GetProcAddress(dllLoaded, lpFunctionName);
    offset = (char*)remoteFunc - (char*)dllLoaded;

    FreeLibrary(dllLoaded);
    return (void*)((DWORD)injBaseAddr + offset);
    }
}

BOOL CallRemoteFunction(HANDLE hOpenProc, const char* dllPath, 
                        LPVOID injBaseAddr, const char* funcName) {
    void* lpInit;

    lpInit = RemoteFunctionExport(dllPath, injBaseAddr, funcName);
    if( lpInit == NULL ) {
        return FALSE;
    }

    HANDLE hThread = CreateRemoteThread(hOpenProc, NULL, 0, lpInit, "test", 0, NULL);

    if(hThread == NULL) {
        return FALSE;
    }

    return TRUE;
}