#include <stdio.h>
#include <TlHelp32.h>
#include <winbase.h>


HANDLE GetProcessHandle(const char *procname) {
    HANDLE hProcessSnap;
    HANDLE hOpenProc;
    PROCESSENTRY32 pe32;
    DWORD *processIds = NULL;
    DWORD procID;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("Invalid handle value\n");
        return 0;
    }

    if(!Process32First(hProcessSnap, &pe32)) {
        printf("Couldn't retrieve first procID\n");
        CloseHandle(hProcessSnap);
        return 0;
    }
    while(Process32Next(hProcessSnap, &pe32)) {
        if (0 == strcasecmp(procname, pe32.szExeFile))
           procID = pe32.th32ProcessID;
    }

    hOpenProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if(hOpenProc == NULL) {
        printf("Failed to open remote process ID: %d\n", (int)procID);
        return 0;
    }

    CloseHandle(hProcessSnap);
    return hOpenProc;
}

LPVOID RemoteOpenAlloc(HANDLE hOpenProc, const char* dllPath) {
    LPVOID baseAddress = NULL;

    baseAddress = (LPVOID)VirtualAllocEx(hOpenProc, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if(baseAddress == NULL) {
        printf("Failed to allocate remote memory\n");
        return NULL;
    }
    return baseAddress;
}

LPDWORD Inject(HANDLE hOpenProc, LPVOID baseAddress, const char* dllPath) {
    LPVOID loadLibA;
    HANDLE threadID;
    HMODULE baseInject;
    int bytesWritten;

    loadLibA = (LPVOID)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
    if(loadLibA == NULL) {
        printf("Failed to locate LoadLibraryA\n");
        return FALSE;
    }

    bytesWritten = WriteProcessMemory(hOpenProc, baseAddress, dllPath, strlen(dllPath) + 1, NULL);

    if(bytesWritten == 0) {
        return FALSE;
    }

    threadID = CreateRemoteThread(hOpenProc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibA, baseAddress, 0, NULL);
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
        return TRUE;
    }
    return TRUE;
}

void* RemoteFunctionExport(LPCWSTR dllPath, HMODULE injBaseAddr, LPCSTR lpFunctionName) {
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
    return (DWORD)injBaseAddr + offset;
    }
}

BOOL CallRemoteFunction(HANDLE hOpenProc, const char* dllPath, LPVOID injBaseAddr, const char* funcName) {
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

void CleanUp(HANDLE hOpenProc, LPVOID allocMemory) {
    CloseHandle(hOpenProc);
    VirtualFreeEx(hOpenProc, (LPVOID)allocMemory, 0, MEM_RELEASE);
}
