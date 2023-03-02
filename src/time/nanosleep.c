#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

#include <psp2/rtc.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	if (!rqtp) {
		errno = EFAULT;
		return -1;
	}

	if (rqtp->tv_sec < 0 || rqtp->tv_nsec < 0 || rqtp->tv_nsec > 999999999) {
		errno = EINVAL;
		return -1;
	}

	const uint32_t us = rqtp->tv_sec * 1000000 + (rqtp->tv_nsec + 999) / 1000;

	sceKernelDelayThread(us);
	return 0;
}
