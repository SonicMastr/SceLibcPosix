#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <sys/mman.h>

#include "vitamman.h"

int mincore(void *addr, size_t len, unsigned char *vec) {
	if (((uintptr_t)addr & (MMAN_PAGE_SIZE - 1)) || !vec) {
		errno = EINVAL;
		return -1;
	}
	if (!len) {
		errno = 0;
		return 0;
	}
	if (!__mman_mapped(addr, len)) {
		errno = ENOMEM;
		return -1;
	}

	memset(vec, 1, MMAN_PAGE_ALIGN(len) / MMAN_PAGE_SIZE);

	errno = 0;
	return 0;
}
