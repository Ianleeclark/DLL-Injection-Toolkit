#include "Detours.h"

int SafeMemcpy(PVOID addr, char *bytes) {
    DWORD tmp;

    int bytes_size = sizeof(bytes) / sizeof(bytes[0]);

    VirtualProtect(addr, bytes_size, PAGE_EXECUTE_READWRITE, &tmp);
    if(memcpy(addr, bytes, bytes_size)) {
        VirtualProtect(addr, bytes_size, tmp, NULL);
        return 0;
    }
    VirtualProtect(addr, bytes_size, tmp, NULL);
    return -1;
}

unsigned int CalculateJmpDistance(unsigned int baseAddress, 
                                  unsigned int remoteAddress) {

    return remoteAddress - baseAddress;
}

int FunctionDetour(LPVOID lpAddress) {
    char jmp_detour[3] = {0x90, 0x90, 0x90};

    SafeMemcpy(lpAddress, jmp_detour);

    return 0;
}