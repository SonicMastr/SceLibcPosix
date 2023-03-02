#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int ftruncate(int fd, off_t length) {
	int ret;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosFHTruncateSync(NULL, fdmap->sce_uid, length);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_DIRECTORY:
	case VITA_DESCRIPTOR_PIPE:
		ret = __make_sce_errno(EBADF);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	errno = 0;
	return 0;
}