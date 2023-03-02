#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

int clock_getres(clockid_t clk_id, struct timespec *res) {
	if (!res) {
		errno = EFAULT;
		return -1;
	}

	switch (clk_id) {
	case CLOCK_REALTIME:
		res->tv_sec = 0;
		res->tv_nsec = 1000; // 1 microsecond
		return 0;
		break;
	case CLOCK_MONOTONIC:
		res->tv_sec = 0;
		res->tv_nsec = 1000; // 1 microsecond
		return 0;
		break;
	default:
		break;
	}

	errno = EINVAL;
	return -1;
}