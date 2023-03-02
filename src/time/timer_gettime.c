#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int timer_gettime(timer_t timerid, struct itimerspec *value) {
	errno = EINVAL;
	return -1;
}