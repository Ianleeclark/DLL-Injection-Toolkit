#include "ProcessHandler.h"

DWORD FindProcID(const char* procname) {
    HANDLE hProcessSnap;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("Invalid handle value\n");
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if(!Process32First(hProcessSnap, &pe32)) {
        printf("Couldn't retrieve first procID\n");
        printf("%d\n", (int)pe32.th32ProcessID);
        CloseHandle(hProcessSnap);
        return 0;
    }

    while(Process32Next(hProcessSnap, &pe32)) {
        if (0 == strcasecmp(procname, pe32.szExeFile)) {
            CloseHandle(hProcessSnap);
            return pe32.th32ProcessID;
        }
    }
    return 0;
}

HANDLE OpenProc(DWORD pid) {
    HANDLE hOpenProc;

    hOpenProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if(hOpenProc == NULL) {
        printf("Failed to open remote process ID: %d\n", (int)pid);
        return NULL;
    }

    return hOpenProc;
}


/*
MODULEENTRY32 FindRemoteModule(HANDLE hOpenProc, const char* modName) {
    //Shoutout to MSDN
    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621%28v=vs.85%29.aspx

    HMODULE hMods[1024];
    DWORD cbNeeded;
    unsigned int i;

    if(EnumProcessModules(hOpenProc, hMods, sizeof(hMods), &cbNeeded)) {
        for(i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];

            if(GetModuleFileNameEx(hOpenProc, hMods[i], szModName,
                                   sizeof(szModName) / sizeof(char*))) {
                if(strcmp(me32.szModule, modName) == 0)
                    return me32;
            }
        }
    }
    return 0;
}
*/

MODULEENTRY32 FindRemoteModule(DWORD pid, const char* modName) {
    HANDLE hModShot;
    MODULEENTRY32 me32;

    hModShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE, pid);
    me32.dwSize = sizeof(MODULEENTRY32);

    if(Module32First(hModShot, &me32)) {
        if(strcmp(me32.szModule, modName) == 0)
            return me32;
        while(Module32Next(hModShot, &me32))
            if(strcmp(me32.szModule, modName) == 0) {
                CloseHandle(hModShot);
                return me32;
            }
    } else 
        me32.dwSize = 0;

    CloseHandle(hModShot);
    return me32;
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

void CleanUp(HANDLE hOpenProc, LPVOID allocMemory) {
    CloseHandle(hOpenProc);
    VirtualFreeEx(hOpenProc, (LPVOID)allocMemory, 0, MEM_RELEASE);
}