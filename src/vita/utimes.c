#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#include <stdlib.h>
#include <psp2/io/stat.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int utimes(const char *filename, const struct timeval times[2]) {
	struct SceFiosStat stat = { 0 };
	char *full_path;
	int ret;

	full_path = __realpath(filename);

	if (!full_path) {
		// errno is set by __realpath
		return -1;
	}

	if (sceFiosStatSync(NULL, full_path, &stat) < 0) {
		goto end;
	}

	if (times) {
		SceDateTime dt[2];
		if (sceRtcConvertTime_tToDateTime(times[0].tv_sec, &dt[0]) < 0) {
			goto end;
		}
		dt[0].microsecond = times[0].tv_usec;
		stat.st_atime = sceFiosDateFromSceDateTime(&dt[0]);
		if (sceRtcConvertTime_tToDateTime(times[1].tv_sec, &dt[1]) < 0) {
			goto end;
		}
		dt[1].microsecond = times[1].tv_usec;
		stat.st_mtime = sceFiosDateFromSceDateTime(&dt[0]);
	} else {
		SceDateTime dt;
		if (sceRtcGetCurrentClockUtc(&dt) < 0) {
			goto end;
		}
		stat.st_atime = sceFiosDateFromSceDateTime(&dt);
		stat.st_mtime = stat.st_atime;
	}

	if (sceFiosChangeStatSync(NULL, full_path, &stat, SCE_CST_AT | SCE_CST_MT) < 0) {
		goto end;
	}

	free(full_path);
	return 0;
end:
	free(full_path);
	errno = __sce_errno_to_errno(ret, ERROR_FIOS);
	return -1;
}