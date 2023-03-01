#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "fios2.h"

extern const char sceUserMainThreadName[] = "test";
extern const int sceUserMainThreadPriority = 0x10000100;
extern const unsigned int sceUserMainThreadStackSize = 0x00040000U;

unsigned int sceLibcHeapSize = 1 * 1024 * 1024;

void __hidden exit_func(void) {
	printf("Exit_func\n");
}

int main(int argc, char const *argv[]) {
	int ret;

	int bytes = 0x01234567;
	int bytes2 = 0;
	int fd = open("ux0:/data/text.txt", O_CREAT | O_RDWR | O_TRUNC);
	int bytes_read = 0;
	int written = 0;
	written = write(fd, &bytes, 4);
	printf("0x%08X\nFD: %d\nWritten: %d\n", bytes, fd, written);
	printf("close: %d\n", close(fd));
	int fd2 = open("app0:sce_module/SceLibcExt.suprx", O_RDONLY);
	bytes_read = read(fd2, &bytes2, 4);
	printf("0x%08X\nFD2: %d\nRead: %d\n", bytes2, fd2, bytes_read);
	int fd3 = open("ux0:/data/text.txt", O_RDONLY);
	bytes_read = read(fd3, &bytes, 4);
	printf("0x%08X\nFD3: %d\nRead: %d\n", bytes, fd3, bytes_read);
	close(fd3);

	atexit(exit_func);
	exit(EXIT_SUCCESS);
	return 0;
}
