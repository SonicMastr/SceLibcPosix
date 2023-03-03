#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

long int sysconf(int name) {
	switch (name) {
	// limits hardcoded for now
	case _SC_ARG_MAX:
		return 65536;
	case _SC_CHILD_MAX:
		return 40;
	case _SC_CLK_TCK:
		return CLK_TCK;
	case _SC_NGROUPS_MAX:
		return 16;
	case _SC_OPEN_MAX:
		return 64;

	// hardcoded values
	case _SC_SEM_NSEMS_MAX:
		return 256;
	case _SC_SEM_VALUE_MAX:
		return 32767;
	case _SC_THREAD_KEYS_MAX:
		return 128;
	case _SC_THREAD_STACK_MIN:
		return 4096;
	case _SC_THREAD_THREADS_MAX:
		return 128;
	case _SC_SEMAPHORES:
		return 1;
	case _SC_THREADS:
		return 1;
	case _SC_THREAD_ATTR_STACKSIZE:
		return 1;
	case _SC_THREAD_SAFE_FUNCTIONS:
		return 1;
	case _SC_VERSION:
		return 200809L; // we (try to) target posix-2008
	default:
		errno = EINVAL;
		return -1;
	}

	return -1;
}