#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <sys/mman.h>

#include "vitamman.h"

void *mremap(void *old_addr, size_t old_len, size_t new_len, int flags, ...) {
	if (((uintptr_t)old_addr & (MMAN_PAGE_SIZE - 1)) || !new_len ||
	    (flags & ~(MREMAP_MAYMOVE | MREMAP_FIXED))) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if ((flags & MREMAP_FIXED) && !(flags & MREMAP_MAYMOVE)) {
		errno = EINVAL;
		return MAP_FAILED;
	}

	void *fixed = NULL;
	if (flags & MREMAP_FIXED) {
		va_list ap;
		va_start(ap, flags);
		fixed = va_arg(ap, void *);
		va_end(ap);
		if ((uintptr_t)fixed & (MMAN_PAGE_SIZE - 1)) {
			errno = EINVAL;
			return MAP_FAILED;
		}
	}

	int prot = __mman_prot_of(old_addr);
	if (prot < 0 || !__mman_mapped(old_addr, old_len)) {
		errno = EFAULT;
		return MAP_FAILED;
	}

	if (!(flags & MREMAP_FIXED) && __mman_resize(old_addr, old_len, new_len) == 0) {
		errno = 0;
		return old_addr;
	}

	if (!(flags & MREMAP_MAYMOVE)) {
		errno = ENOMEM;
		return MAP_FAILED;
	}

	int err = 0;
	void *p = __mman_map(fixed, new_len, prot | PROT_WRITE, &err);
	if (!p) {
		errno = err ? err : ENOMEM;
		return MAP_FAILED;
	}

	size_t copy = old_len < new_len ? old_len : new_len;
	memcpy(p, old_addr, copy);

	if ((prot | PROT_WRITE) != prot)
		__mman_protect(p, new_len, prot);

	__mman_unmap(old_addr, old_len);

	errno = 0;
	return p;
}
