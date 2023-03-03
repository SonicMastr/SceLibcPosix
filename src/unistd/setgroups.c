#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int setgroups(int size, const gid_t *list) {
	errno = ENOSYS;
	return -1;
}