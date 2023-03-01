#include <sys/unistd.h>
#include <errno.h>

#include "vitadescriptor.h"

int close(int fd) {
	int res = __release_descriptor(fd);
	if (res < 0) {
		errno = EBADF;
		return -1;
	}
	errno = 0;
	return 0;
}
