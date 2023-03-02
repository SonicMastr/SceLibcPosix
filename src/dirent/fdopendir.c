#include <sys/dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"

DIR *fdopendir(int fd) {
	DescriptorTranslation *fdmap = __fd_grab(fd);
	if (!fdmap) {
		errno = EBADF;
		return NULL;
	}

	if (fdmap->type != VITA_DESCRIPTOR_DIRECTORY) {
		__fd_drop(fdmap);
		errno = ENOTDIR;
		return NULL;
	}

	__fd_drop(fdmap);

	return (__opendir_common(fd));
}
