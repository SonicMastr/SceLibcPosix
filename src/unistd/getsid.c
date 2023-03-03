#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

pid_t getsid(pid_t pid) {
	errno = ENOSYS;
	return -1;
}