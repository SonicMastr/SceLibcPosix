#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int stat(const char *path, struct stat *st) {
	struct SceFiosStat stat = { 0 };
	int ret;
	char *full_path = __realpath(path);
	if (!full_path) {
		return -1;
	}
	if ((ret = sceFiosStatSync(NULL, full_path, &stat)) < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}
	free(full_path);
	__scestat_to_stat(&stat, st);
	errno = 0;
	return 0;
}