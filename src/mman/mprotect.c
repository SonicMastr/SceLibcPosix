#include <errno.h>
#include <stdint.h>

#include <sys/mman.h>

#include "vitamman.h"

#include "vitaerror.h"

int mprotect(void *addr, size_t len, int prot) {
	uintptr_t start = (uintptr_t)addr & ~(uintptr_t)(MMAN_PAGE_SIZE - 1);
	uintptr_t end = MMAN_PAGE_ALIGN((uintptr_t)addr + len);
	int ret;

	if (!len) {
		errno = EINVAL;
		return -1;
	}

	if ((ret = __mman_protect((void *)start, end - start, prot)) < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	errno = 0;
	return 0;
}
