#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fios2.h"

int *_sceLibcErrnoLoc(void);
#define errno (*_sceLibcErrnoLoc())

#include <psp2/kernel/processmgr.h>

extern const char sceUserMainThreadName[] = "test";
extern const int sceUserMainThreadPriority = 0x10000100;
extern const unsigned int sceUserMainThreadStackSize = 0x00040000U;

unsigned int sceLibcHeapSize = 32 * 1024 * 1024;

void __hidden exit_func(void) {
	printf("Exit_func\n");
}

int main(int argc, char const *argv[]) {
	int ret;

	int elf;
	FILE *file = fopen("/sce_module/SceLibcPosix.suprx", "rb");
	fread(&elf, 4, 1, file);
	printf("elf out: 0x%08X\n", elf);
	fclose(file);

	atexit(exit_func);
	exit(EXIT_SUCCESS);
	return 0;
}
