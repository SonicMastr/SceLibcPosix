#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#include "vitamman.h"

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	int anon = (flags & MAP_ANONYMOUS) != 0;
	int err = 0;

	if (!len || off < 0 || (off & (MMAN_PAGE_SIZE - 1))) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (!(flags & (MAP_SHARED | MAP_PRIVATE))) {
		errno = EINVAL;
		return MAP_FAILED;
	}

	if (anon) {
		fd = -1;
	} else {
		if (fd < 0) {
			errno = EBADF;
			return MAP_FAILED;
		}
		if ((flags & MAP_SHARED) && (prot & PROT_WRITE)) {
			errno = ENOTSUP;
			return MAP_FAILED;
		}
	}

	void *base = __mman_map((flags & MAP_FIXED) ? addr : NULL, len, prot | PROT_WRITE, &err);
	if (!base) {
		errno = err ? err : ENOMEM;
		return MAP_FAILED;
	}

	if (anon) {
		memset(base, 0, MMAN_PAGE_ALIGN(len));
	} else {
		size_t done = 0;
		while (done < len) {
			ssize_t n = pread(fd, (char *)base + done, len - done, off + done);
			if (n < 0) {
				int e = errno;
				__mman_unmap(base, len);
				errno = e;
				return MAP_FAILED;
			}
			if (n == 0)
				break;
			done += (size_t)n;
		}
		memset((char *)base + done, 0, MMAN_PAGE_ALIGN(len) - done);
	}

	if ((prot | PROT_WRITE) != prot)
		__mman_protect(base, len, prot);

	errno = 0;
	return base;
}
