#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

struct dirent *readdir(DIR *dirp)
{
	SceFiosDirEntry tempdir;
	if (!dirp) {
		errno = EBADF;
		return NULL;
	}

	DescriptorTranslation *fdmap = __fd_grab(dirp->fd);
	if (!fdmap) {
		errno = EBADF;
		return NULL;
	}

	int res = sceFiosDHReadSync(NULL, fdmap->sce_uid, (SceFiosDirEntry *)&tempdir);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __sce_errno_to_errno(res, ERROR_FIOS);
		return NULL;
	}

	dirp->dir.d_ino = 0;
	strncpy(dirp->dir.d_name, &tempdir.fullPath[tempdir.offsetToName], tempdir.nameLength);

	if (res == 0) {
		// end-of-listing shouldn't change errno
		return NULL;
	}

	struct dirent *dir = &dirp->dir;
	dirp->index++;
	return dir;
}