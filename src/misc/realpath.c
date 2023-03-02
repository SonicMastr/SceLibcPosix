#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>
#include <string.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

char *realpath(const char *path, char *resolved_path) {
	char *fullpath = __realpath(path);
	if (!fullpath) {
		return NULL; // errno already set
	}

	SceFiosStat stat;
	int ret = sceFiosStatSync(NULL, fullpath, &stat);
	if (ret < 0) {
		free(fullpath);
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return NULL;
	}

	if (!resolved_path) {
		resolved_path = fullpath;
	} else {
		strcpy(resolved_path, fullpath);
		free(fullpath);
	}

	return resolved_path;
}