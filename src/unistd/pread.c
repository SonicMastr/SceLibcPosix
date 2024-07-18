#include <errno.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

ssize_t pread(int fd, void *buf, size_t size, off_t off) {
	int ret;
	int type = ERROR_GENERIC;
	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = __make_sce_errno(EBADF);
		break;
	case VITA_DESCRIPTOR_FILE:
		type = ERROR_FIOS;
		ret = sceFiosFHPreadSync(NULL, fdmap->sce_uid, buf, size, off);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		ret = sceIoPread(fdmap->sce_uid, buf, size, off);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			errno = __sce_errno_to_errno(ret, type);
		return -1;
	}

	errno = 0;
	return ret;
}