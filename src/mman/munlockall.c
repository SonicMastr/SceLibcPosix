#include <errno.h>

#include <sys/mman.h>

int munlockall(void) {
	errno = 0;
	return 0;
}
