#include <errno.h>
#include <stdlib.h>

int posix_memalign(void **res, size_t align, size_t len) {
	if (align < sizeof(void *) || (align & (align - 1)))
		return EINVAL;

	void *mem = aligned_alloc(align, len);
	if (!mem)
		return ENOMEM;

	*res = mem;
	return 0;
}
