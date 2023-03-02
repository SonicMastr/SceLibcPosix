#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fchdir(int fd) {
	int ret;
	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		__fd_drop(fdmap);
		errno = ENOTDIR;
		return -1;
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = chdir(sceFiosDHGetPath(fdmap->sce_uid));
		__fd_drop(fdmap);
		return ret;
	}
	return 0;
}