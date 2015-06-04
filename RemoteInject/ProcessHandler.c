#include "ProcessHandler.h"


HANDLE GetProcessHandle(const char *procname) {
    HANDLE hProcessSnap;
    HANDLE hOpenProc;
    PROCESSENTRY32 pe32;
    DWORD procID;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("Invalid handle value\n");
        return 0;
    }

    pe32.dwSize = sizeof(pe32);
    if(!Process32First(hProcessSnap, &pe32)) {
        printf("Couldn't retrieve first procID\n");
        printf("%d\n", (int)pe32.th32ProcessID);
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

    baseAddress = (LPVOID)VirtualAllocEx(hOpenProc, NULL, strlen(dllPath) + 1,
                                         MEM_COMMIT, PAGE_READWRITE);
    if(baseAddress == NULL) {
        printf("Failed to allocate remote memory\n");
        return NULL;
    }
    return baseAddress;
}

int FindBaseAddress(HANDLE hProcess, LPCVOID lpAddress) {
    MEMORY_BASIC_INFORMATION mbi;

    if(VirtualQueryEx(hProcess, lpAddress, &mbi, sizeof(mbi)) < sizeof(mbi)) {
        printf("Failed to Query remote process\n");
        return -1;
    }

    printf("%X\n", (unsigned int)mbi.BaseAddress);
    return 0;
}

void CleanUp(HANDLE hOpenProc, LPVOID allocMemory) {
    CloseHandle(hOpenProc);
    VirtualFreeEx(hOpenProc, (LPVOID)allocMemory, 0, MEM_RELEASE);
}
