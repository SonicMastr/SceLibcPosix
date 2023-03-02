#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fsync(int fd) {
	int ret = 0;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = sceFiosSyncSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), 0);
		break;
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosFHSyncSync(NULL, fdmap->sce_uid);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		ret = __make_sce_errno(EINVAL);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}
	return 0;
}
