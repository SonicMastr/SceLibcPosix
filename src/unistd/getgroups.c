#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int getgroups(int size, gid_t list[]) {
	errno = ENOSYS;
	return -1;
}