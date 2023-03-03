#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int setreuid(uid_t ruid, uid_t euid) {
	errno = ENOSYS;
	return -1;
}