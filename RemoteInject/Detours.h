#ifndef DETOURS_H
#define DETOURS_H

#include <stdio.h>
#include <winbase.h>

int safe_memcpy(PVOID, char*);
int FunctionDetour();

#endif // DETOURS_H