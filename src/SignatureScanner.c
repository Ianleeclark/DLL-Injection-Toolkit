#include "SignatureScanner.h"

void* SignatureScan(DWORD BaseAddress, DWORD BaseFileSize,
				    BYTE* pattern, const char* mask) {
	unsigned char* baseaddr;
	unsigned char* endaddr;
	size_t len = strlen((const char*)pattern);
	size_t i;

	baseaddr = (unsigned char*)BaseAddress;
	endaddr = (unsigned char*)(BaseAddress + BaseFileSize);

	while(baseaddr < endaddr) {
		for(i = 0; i < len; i++) {
			if((mask[i] != '?') && (pattern[i] != baseaddr[i]))
				break;
		}

		if(i == len)
			return (void*)baseaddr;

		baseaddr++;
	}
	return NULL;
}