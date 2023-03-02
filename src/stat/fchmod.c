#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <psp2/io/stat.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fchmod(int fd, mode_t mode) {
	struct SceFiosStat stat = { 0 };
	int ret = 0;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
		ret = chmod(sceFiosFHGetPath(fdmap->sce_uid), mode);
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = chmod(sceFiosDHGetPath(fdmap->sce_uid), mode);
		break;
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		ret = __make_sce_errno(EBADF);
		break;
	}

	if (ret < 0) {
		__fd_drop(fdmap);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	stat.st_mode = mode;

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosChangeStatSync(NULL, sceFiosFHGetPath(fdmap->sce_uid), &stat, SCE_CST_MODE);
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = sceFiosChangeStatSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), &stat, SCE_CST_MODE);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	return 0;
}