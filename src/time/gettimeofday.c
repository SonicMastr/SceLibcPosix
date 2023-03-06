#include <time.h>
#include <sys/time.h>

int gettimeofday(struct timeval *ptimeval, void *ptimezone) {
	if (!ptimeval)
		return 0;
	int ret = sceKernelLibcGettimeofday(ptimeval, ptimezone);
	return 0;
}