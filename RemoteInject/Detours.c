#include "Detours.h"

int safe_memcpy(PVOID addr, char *bytes) {
    DWORD tmp;

    int bytes_size = sizeof(bytes)/sizeof(bytes[0]);

    VirtualProtect(addr, bytes_size, PAGE_EXECUTE_READWRITE, &tmp);
    if(memcpy(addr, bytes, bytes_size)) {
        VirtualProtect(addr, bytes_size, tmp, NULL);
        return 0;
    }
    VirtualProtect(addr, bytes_size, tmp, NULL);
    return -1;
}

int FunctionDetour() {
    char jmp_detour[3] = {0xEB, 0x19, 0x90};

    safe_memcpy((PVOID)0x0401483, (char*)jmp_detour);

    return 0;
}