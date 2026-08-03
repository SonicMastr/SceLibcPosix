#include <errno.h>
#include <stdint.h>

#include <sys/mman.h>

#include "vitamman.h"

int posix_madvise(void *addr, size_t len, int advice) {
	if ((uintptr_t)addr & (MMAN_PAGE_SIZE - 1))
		return EINVAL;
	if (!len)
		return 0;

	switch (advice) {
	case POSIX_MADV_NORMAL:
	case POSIX_MADV_RANDOM:
	case POSIX_MADV_SEQUENTIAL:
	case POSIX_MADV_WILLNEED:
	case POSIX_MADV_DONTNEED:
		break;
	default:
		return EINVAL;
	}

	if (!__mman_mapped(addr, len))
		return ENOMEM;

	return 0;
}
