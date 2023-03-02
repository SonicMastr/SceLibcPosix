#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#include <stdlib.h>
#include <psp2/io/stat.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

// Currently not Posix Compatible, but most openat() based functions aren't anyway so I'm gonna put this here
int utimensat(int fd, const char *path, const struct timespec times[2], int flags) {
	struct SceFiosStat stat = { 0 };
	unsigned int bits = 0;
	int ret;
	int is_dir = 0;

	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_DIRECTORY:
		is_dir = 1;
		ret = sceFiosStatSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), &stat);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_FILE:
		ret = sceFiosFHStatSync(NULL, fdmap->sce_uid, &stat);
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

	if (times) {
		SceDateTime dt[2];
		for (int i = 0; i < 2; i++) {
			if ((times[i].tv_nsec != UTIME_NOW) && (times[i].tv_nsec != UTIME_OMIT)) {
				ret = sceRtcConvertTime_tToDateTime(times[i].tv_sec, &dt[i]);
				dt[i].microsecond = times[i].tv_nsec / 1000;
			} else {
				ret = sceRtcGetCurrentClockUtc(&dt[i]);
			}
			if (ret < 0) {
				__fd_drop(fdmap);
				errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
				return -1;
			}
		}
		stat.st_atime = sceFiosDateFromSceDateTime(&dt[0]);
		stat.st_mtime = sceFiosDateFromSceDateTime(&dt[1]);
	} else {
		SceDateTime dt;
		ret = sceRtcGetCurrentClockUtc(&dt);
		if (ret < 0) {
			__fd_drop(fdmap);
			errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
			return -1;
		}
		stat.st_atime = sceFiosDateFromSceDateTime(&dt);
		stat.st_mtime = stat.st_atime;
	}

	if (!times || times[0].tv_nsec != UTIME_OMIT)
		bits |= SCE_CST_AT;
	if (!times || times[1].tv_nsec != UTIME_OMIT)
		bits |= SCE_CST_MT;

	if (is_dir)
		ret = sceFiosChangeStatSync(NULL, sceFiosDHGetPath(fdmap->sce_uid), &stat, bits);
	else
		ret = sceFiosChangeStatSync(NULL, sceFiosFHGetPath(fdmap->sce_uid), &stat, bits);

	__fd_drop(fdmap);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	return 0;
}