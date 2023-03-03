#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <psp2/kernel/threadmgr.h>
#include "vitadescriptor.h"
#include "vitamem.h"

int setrlimit(int resource, const struct rlimit *rlim) {
	// TODO: implement or return EINVAL?
	errno = ENOSYS;
	return -1;
}