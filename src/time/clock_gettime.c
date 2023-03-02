#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	time_t seconds;
	SceDateTime time;
	uint64_t t;

	if (!tp) {
		errno = EFAULT;
		return -1;
	}

	switch (clk_id) {
	case CLOCK_REALTIME:
		sceRtcGetCurrentClock(&time, 0);
		sceRtcGetTime_t(&time, &seconds);

		tp->tv_sec = seconds;
		tp->tv_nsec = time.microsecond * 1000;
		return 0;
		break;
	case CLOCK_MONOTONIC:
		t = sceKernelGetProcessTimeWide();
		tp->tv_sec = t / 1000000;
		tp->tv_nsec = (t % 1000000) * 1000;
		return 0;
		break;
	default:
		break;
	}

	errno = EINVAL;
	return -1;
}