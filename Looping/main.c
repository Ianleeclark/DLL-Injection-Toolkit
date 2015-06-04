#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "ProcessHandler.c"
#include "DLLInjection.c"
#include "Detours.c"


int funcA() {
	printf("TestA\n");
	return 0;
}

int funcB() {
	printf("TestB\n");
	return 0;
}

int main()
{
    int i;
    for(i = 1; i <= 3; i++) {
        funcA();
        getchar();
    }
    return 0;
}
