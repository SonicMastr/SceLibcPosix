#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid) {
	errno = ENOSYS;
	return -1;
}