#ifndef DETOURS_H
#define DETOURS_H

#include <stdio.h>
#include <winbase.h>

int SafeMemcpy(PVOID, char*);
int FunctionDetour();

unsigned int CalculateJmpDistance(unsigned int, unsigned int);

#endif // DETOURS_H