#include <errno.h>
#include <stdint.h>

#include <sys/mman.h>

#include "vitamman.h"

int madvise(void *addr, size_t len, int advice) {
	if ((uintptr_t)addr & (MMAN_PAGE_SIZE - 1)) {
		errno = EINVAL;
		return -1;
	}
	if (!len) {
		errno = 0;
		return 0;
	}

	switch (advice) {
	case MADV_DONTNEED:
	case MADV_FREE:
		if (!__mman_mapped(addr, len)) {
			errno = ENOMEM;
			return -1;
		}
		if (__mman_discard(addr, len) < 0) {
			errno = ENOMEM;
			return -1;
		}
		break;

	case MADV_NORMAL:
	case MADV_RANDOM:
	case MADV_SEQUENTIAL:
	case MADV_WILLNEED:
	case MADV_DONTFORK:
	case MADV_DOFORK:
	case MADV_MERGEABLE:
	case MADV_UNMERGEABLE:
	case MADV_HUGEPAGE:
	case MADV_NOHUGEPAGE:
	case MADV_DONTDUMP:
	case MADV_DODUMP:
	case MADV_WIPEONFORK:
	case MADV_KEEPONFORK:
		if (!__mman_mapped(addr, len)) {
			errno = ENOMEM;
			return -1;
		}
		break;

	default:
		errno = EINVAL;
		return -1;
	}

	errno = 0;
	return 0;
}
