#include <errno.h>

#include <sys/mman.h>

#include "vitamman.h"

int munmap(void *addr, size_t len) {
	if (!len || ((uintptr_t)addr & (MMAN_PAGE_SIZE - 1))) {
		errno = EINVAL;
		return -1;
	}

	if (__mman_unmap(addr, len) < 0) {
		errno = EINVAL;
		return -1;
	}

	errno = 0;
	return 0;
}
