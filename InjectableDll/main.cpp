#include <windows.h>
#include <stdio.h>
BOOL APIENTRY DllMain (HINSTANCE hInst,DWORD reason,LPVOID reserved){

    return TRUE;
}

extern "C" __declspec(dllexport) void testFunc(void) {
    FILE* file;
    file = fopen("f:\\test.txt", "a+");
    fprintf(file, TEXT("TEST FUNCTION PLEASE IGNORE\n"));
    fclose(file);
}
