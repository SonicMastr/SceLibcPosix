#include <errno.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int fstatvfs(int __fd, struct statvfs *__buf) {
	int ret;
	DescriptorTranslation *fdmap = __fd_grab(__fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		__fd_drop(fdmap);
		memset(__buf, 0, sizeof(struct statvfs));
		__buf->f_bsize = 4096;
		__buf->f_frsize = 4096;
		__buf->f_namemax = 256;
		return 0;
	case VITA_DESCRIPTOR_FILE:
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = statvfs(sceFiosDHGetPath(fdmap->sce_uid), __buf);
		__fd_drop(fdmap);
		return ret;
	}
	return 0;
}
