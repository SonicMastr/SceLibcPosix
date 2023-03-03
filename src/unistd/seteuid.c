#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int seteuid(uid_t euid) {
	errno = ENOSYS;
	return -1;
}