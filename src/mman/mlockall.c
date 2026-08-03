#include <errno.h>

#include <sys/mman.h>

int mlockall(int flags) {
	if (!(flags & (MCL_CURRENT | MCL_FUTURE)) ||
	    (flags & ~(MCL_CURRENT | MCL_FUTURE | MCL_ONFAULT))) {
		errno = EINVAL;
		return -1;
	}

	errno = 0;
	return 0;
}
