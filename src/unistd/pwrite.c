#include <errno.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <psp2/io/fcntl.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

ssize_t pwrite(int __fd, const void *__buf, size_t __nbytes, off_t __offset) {
	int ret;
	DescriptorTranslation *fdmap = __fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = __make_sce_errno(EBADF);
		break;
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosFHPwriteSync(NULL, fdmap->sce_uid, __buf, __nbytes, __offset);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		ret = sceIoPwrite(fdmap->sce_uid, __buf, __nbytes, __offset);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	errno = 0;
	return ret;
}