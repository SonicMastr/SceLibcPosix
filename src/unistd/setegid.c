#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int setegid(gid_t gid) {
	errno = ENOSYS;
	return -1;
}