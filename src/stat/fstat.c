#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fstat(int fd, struct stat *st) {
	struct SceFiosStat stat = { 0 };
	int ret;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		if (0 > sceFiosFHTell(fd)) { // C++ is a bit weird
			errno = EBADF;
			return -1;
		}
		ret = sceFiosFHStatSync(NULL, fd, &stat);
	} else {
		switch (fdmap->type) {
		case VITA_DESCRIPTOR_DIRECTORY:
			ret = sceFiosStatSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), &stat);
			break;
		case VITA_DESCRIPTOR_FILE:
			ret = sceFiosFHStatSync(NULL, fdmap->sce_uid, &stat);
			break;
		case VITA_DESCRIPTOR_TTY:
		case VITA_DESCRIPTOR_SOCKET:
		case VITA_DESCRIPTOR_PIPE:
			ret = __make_sce_errno(EBADF);
			break;
		}

		__fd_drop(fdmap);
	}

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	__scestat_to_stat(&stat, st);
	errno = 0;
	return 0;
}