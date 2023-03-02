#include <sys/dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"

int closedir(DIR *dirp) {
	if (!dirp || !dirp->fd) {
		errno = EBADF;
		return -1;
	}

	int res = close(dirp->fd);
	free(dirp);

	if (res < 0) {
		return -1;
	}

	errno = 0;
	return 0;
}