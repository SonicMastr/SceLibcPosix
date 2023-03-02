#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <psp2/io/stat.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int chmod(const char *pathname, mode_t mode) {
	struct SceFiosStat stat = { 0 };
	char *full_path = __realpath(pathname);
	if (!full_path) {
		// errno is set by __realpath
		return -1;
	}
	int ret = sceFiosStatSync(NULL, full_path, &stat);
	if (ret < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	stat.st_mode = mode;
	ret = sceFiosChangeStatSync(NULL, full_path, &stat, SCE_CST_MODE);
	if (ret < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}
	free(full_path);
	return 0;
}
