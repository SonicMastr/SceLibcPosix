#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fios2.h"

int *_sceLibcErrnoLoc(void);
#define errno (*_sceLibcErrnoLoc())

#include <psp2/kernel/processmgr.h>

extern int sceLibcGetFD(FILE *) __attribute__((weak));
extern FILE *sceLibcGetFH(int) __attribute__((weak));

extern const char sceUserMainThreadName[] = "test";
extern const int sceUserMainThreadPriority = 0x10000100;
extern const unsigned int sceUserMainThreadStackSize = 0x00040000U;

unsigned int sceLibcHeapSize = 1 * 1024 * 1024;

void __hidden exit_func(void) {
	printf("Exit_func\n");
}

int main(int argc, char const *argv[]) {
	int ret;

	char output[256] __attribute__((__aligned__(64)));
	SceFiosSize outputSize = 0;
	SceFiosSize inputSize = 0;
	SceFiosSize result = 0;
	SceFiosFH writeFH = 0;
	SceFiosOp op[1] = { 0 };
	SceFiosOpenParams openParams = SCE_FIOS_OPENPARAMS_INITIALIZER;

	openParams.openFlags = SCE_FIOS_O_WRONLY;

	strncpy(output, "SAMPLE OUTPUT\n", 256);
	outputSize = (SceFiosSize)(strlen(output) + 1);

	int fd = open("app0:sce_module/SceLibcExt.suprx", O_RDWR | O_CREAT);

	printf("fd %d\nerror: %d\n", fd, errno);

	ret = read(fd, NULL, 3);

	printf("ret 0x%08X\nerror: %d\n", ret, errno);

	atexit(exit_func);
	exit(EXIT_SUCCESS);
	return 0;
}
