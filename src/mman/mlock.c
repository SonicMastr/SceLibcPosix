#include <errno.h>
#include <stdint.h>

#include <sys/mman.h>

#include "vitamman.h"

int mlock(const void *addr, size_t len) {
	if (!len) {
		errno = 0;
		return 0;
	}
	if (!__mman_mapped((void *)addr, len)) {
		errno = ENOMEM;
		return -1;
	}

	errno = 0;
	return 0;
}
