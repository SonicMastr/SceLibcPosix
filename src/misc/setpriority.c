#include <errno.h>
#include <sys/resource.h>

int setpriority(int which, id_t who, int prio) {
	// TODO: implement
	errno = ENOSYS;
	return -1;
}