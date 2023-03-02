#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int timer_getoverrun(timer_t timerid) {
	errno = EINVAL;
	return -1;
}