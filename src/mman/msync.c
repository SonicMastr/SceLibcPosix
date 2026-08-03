#include <errno.h>
#include <stdint.h>

#include <sys/mman.h>

#include "vitamman.h"

#include "vitaerror.h"

int msync(void *addr, size_t len, int flags) {
	uintptr_t start = (uintptr_t)addr & ~(uintptr_t)(MMAN_PAGE_SIZE - 1);
	uintptr_t end = MMAN_PAGE_ALIGN((uintptr_t)addr + len);
	int ret;

	if (!len || (flags & ~(MS_ASYNC | MS_INVALIDATE | MS_SYNC))) {
		errno = EINVAL;
		return -1;
	}
	if ((flags & (MS_ASYNC | MS_SYNC)) == (MS_ASYNC | MS_SYNC)) {
		errno = EINVAL;
		return -1;
	}

	if ((ret = __mman_sync((void *)start, end - start)) < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	errno = 0;
	return 0;
}
