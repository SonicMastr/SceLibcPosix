#include <errno.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <stdlib.h>
#include <psp2/io/stat.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int statvfs(const char *__path, struct statvfs *__buf) {
	if (!__buf || !__path) {
		errno = EFAULT;
		return -1;
	}

	char *realpath = __realpath(__path);
	if (!realpath) {
		return -1; // errno already set
	}

	char drive[16] = { 0 };
	int d = __get_drive(realpath);
	if (d > 15)
		d = 15; // minus zero-termination
	strncpy(drive, realpath, d);

	SceIoDevInfo info;
	memset(&info, 0, sizeof(SceIoDevInfo));
	int ret = sceFiosDevctlSync(NULL, drive, 0x3001, NULL, 0, &info, sizeof(SceIoDevInfo));
	if (ret < 0) {
		errno = EIO;
		return -1;
	}

	__buf->f_bsize = info.cluster_size;
	__buf->f_frsize = info.cluster_size;

	__buf->f_blocks = info.max_size / info.cluster_size;
	__buf->f_bfree = info.free_size / info.cluster_size;
	__buf->f_bavail = info.free_size / info.cluster_size;

	__buf->f_files = ULONG_MAX;
	__buf->f_ffree = ULONG_MAX;
	__buf->f_favail = ULONG_MAX;

	__buf->f_fsid = 0; // zero for now. requires syncing with stat_t.st_dev
	__buf->f_flag = ST_NOSUID;
	__buf->f_namemax = 256;
}
