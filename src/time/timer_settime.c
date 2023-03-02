#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int timer_settime(timer_t timerid, int flags,
				  const struct itimerspec *__restrict value,
				  struct itimerspec *__restrict ovalue) {
	errno = EINVAL;
	return -1;
}