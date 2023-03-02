#include <errno.h>
#include "__dirent.h"

int dirfd(DIR *dirp) {
	if (!dirp) {
		errno = EINVAL;
		return -1;
	}

	return dirp->fd;
}