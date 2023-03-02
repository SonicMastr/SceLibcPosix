#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

void rewinddir(DIR *dirp) {
	int ret;
	SceFiosDH handle = 0;
	SceFiosBuffer buf = SCE_FIOS_BUFFER_INITIALIZER;

	if (!dirp) {
		errno = EBADF;
		return;
	}

	DescriptorTranslation *fdmap = __fd_grab(dirp->fd);
	if (!fdmap) {
		errno = EBADF;
		return;
	}

	SceUID dirfd = sceFiosDHOpenSync(NULL, &handle, sceFiosDHGetPath(fdmap->sce_uid), buf);
	dirfd = dirfd < 0 ? dirfd : handle;

	if (dirfd < 0) {
		__fd_drop(fdmap);
		errno = __sce_errno_to_errno(dirfd, ERROR_FIOS);
		return;
	}

	sceFiosDHCloseSync(NULL, fdmap->sce_uid);
	fdmap->sce_uid = dirfd;
	__fd_drop(fdmap);

	dirp->index = 0;
	errno = 0;
}
