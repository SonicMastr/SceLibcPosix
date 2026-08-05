#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

void *aligned_alloc(size_t align, size_t len) {
	if (!align || (align & (align - 1))) {
		errno = EINVAL;
		return NULL;
	}

	void *mem = memalign(align, len);
	if (!mem) {
		errno = ENOMEM;
		return NULL;
	}

	return mem;
}
