#include <errno.h>
#include <unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"

_off_t lseek(int fd, _off_t ptr, int dir) {
	int ret;
	DescriptorTranslation *fdmap = __fd_grab(fd);
	if (!fdmap) {
		if (0 > sceFiosFHTell(fd)) { // C++ is a bit weird
			errno = EBADF;
			return -1;
		}
		ret = sceFiosFHSeek(fd, ptr, dir);
	} else {
		switch (fdmap->type) {
		case VITA_DESCRIPTOR_FILE:
			ret = sceFiosFHSeek(fdmap->sce_uid, ptr, dir);
			break;
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = __make_sce_errno(EBADF);
			break;
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_SOCKET:
		case VITA_DESCRIPTOR_PIPE:
			ret = __make_sce_errno(ESPIPE); // Yes, this is the correct error code
			break;
		}
		__fd_drop(fdmap);
	}
	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}
	return ret;
}