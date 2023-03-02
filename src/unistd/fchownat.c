#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fchownat(int fd, const char *path, uid_t owner, gid_t group, int flag) {
	// Implementation note: there's no real chown on vita
	// We only check for path correctness
	struct SceFiosStat stat = { 0 };
	int ret = 0;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosStatSync(NULL, sceFiosFHGetPath(fdmap->sce_uid), &stat);
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = sceFiosStatSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), &stat);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
		ret = __make_sce_errno(EBADF);
		break;
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	return 0;
}
