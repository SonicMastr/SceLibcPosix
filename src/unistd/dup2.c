#include <errno.h>
#include "vitadescriptor.h"

int dup2(int oldfd, int newfd) {

    if (newfd < 0 || newfd >= MAX_OPEN_FILES)
    {
        errno = EBADF;
        return -1;
    }

	int fd = __duplicate_descriptor2(oldfd, newfd);

	if (fd < 0) {
		errno = EBADF;
		return -1;
	}

	errno = 0;
	return fd;
}