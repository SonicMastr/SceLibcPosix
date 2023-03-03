#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <psp2/kernel/threadmgr.h>
#include "vitadescriptor.h"

int getrusage(int who, struct rusage *usage) {
	if (usage == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (who = RUSAGE_SELF) {
		SceKernelThreadInfo info = { 0 };
		info.size = sizeof(SceKernelThreadInfo);
		sceKernelGetThreadInfo(0, &info);

		usage->ru_utime.tv_sec = info.runClocks / 1000000;
		usage->ru_utime.tv_usec = info.runClocks - (info.runClocks / 1000000);
		usage->ru_stime.tv_sec = 0;
		usage->ru_stime.tv_usec = 0;
	}
	errno = EINVAL;
	return -1;
}