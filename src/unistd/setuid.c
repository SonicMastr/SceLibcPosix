#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int setuid(uid_t uid) {
	errno = ENOSYS;
	return -1;
}