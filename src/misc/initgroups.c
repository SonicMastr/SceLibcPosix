#include <errno.h>
#include <stdio.h>
#include <sys/types.h>

int initgroups(const char *user, gid_t group) {
	errno = ENOSYS;
	return -1;
}