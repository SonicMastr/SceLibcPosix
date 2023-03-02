#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int clock_settime(clockid_t clk_id, const struct timespec *tp) {
	if (!tp) {
		errno = EFAULT;
		return -1;
	}

	errno = EPERM;
	return -1;
}
