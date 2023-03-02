#include <errno.h>
#include "__dirent.h"

long int telldir(DIR *dirp) {
	if (!dirp) {
		errno = EBADF;
		return -1;
	}

	return dirp->index;
}