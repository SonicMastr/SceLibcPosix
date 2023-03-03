#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <psp2/kernel/threadmgr.h>
#include "vitadescriptor.h"
#include "vitamem.h"

int getrlimit(int resource, struct rlimit *rlim) {
	if (rlim == NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (resource) {
	case RLIMIT_CORE:
		rlim->rlim_cur = 0;
		rlim->rlim_max = 0;
		return 0;
	case RLIMIT_CPU:
		rlim->rlim_cur = RLIM_INFINITY;
		rlim->rlim_max = RLIM_INFINITY;
		return 0;
	case RLIMIT_DATA: // newlib heap size
	{
		unsigned int heap_size = _get_heap_size();
		rlim->rlim_cur = heap_size;
		rlim->rlim_max = heap_size;
		return 0;
	}
	case RLIMIT_FSIZE: // Technically, 16 exabytes
		rlim->rlim_cur = RLIM_INFINITY;
		rlim->rlim_max = RLIM_INFINITY;
		return 0;
	case RLIMIT_NOFILE:
		rlim->rlim_cur = MAX_OPEN_FILES;
		rlim->rlim_max = MAX_OPEN_FILES;
		return 0;
	case RLIMIT_STACK: // get from thread
	{
		SceKernelThreadInfo info = { 0 };
		info.size = sizeof(SceKernelThreadInfo);
		sceKernelGetThreadInfo(0, &info);
		rlim->rlim_cur = info.stackSize;
		rlim->rlim_max = info.stackSize;
		return 0;
	}
	case RLIMIT_AS: // virtual address space
		rlim->rlim_cur = RLIM_INFINITY;
		rlim->rlim_max = RLIM_INFINITY;
		return 0;
	default:
		errno = EINVAL;
		return -1;
	}
}