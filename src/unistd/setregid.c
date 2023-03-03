#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int setregid(gid_t rgid, gid_t egid) {
	errno = ENOSYS;
	return -1;
}