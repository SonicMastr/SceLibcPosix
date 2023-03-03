#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <psp2/kernel/rng.h>

#define MAX_ENTROPY 256

int getentropy(void *ptr, size_t n) {
	size_t target = n;
	void *p = ptr;
	int ret;

	if (n > MAX_ENTROPY) {
		errno = EIO;
		return -1;
	}

	while (target > 64) {
		ret = sceKernelGetRandomNumber(ptr, 64);
		ptr += 64;
		target -= 64;
		if (ret < 0) {
			errno = EIO;
			return -1;
		}
	}

	ret = sceKernelGetRandomNumber(ptr, target);
	if (ret < 0) {
		errno = EIO;
		return -1;
	}

	return 0;
}