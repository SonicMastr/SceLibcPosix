#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

pid_t setsid() {
	errno = ENOSYS;
	return -1;
}