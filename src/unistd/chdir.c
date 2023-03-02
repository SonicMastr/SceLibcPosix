#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int chdir(const char *path) {
	char *fullpath = NULL;

	if (!path) // different error
	{
		errno = EFAULT;
		return -1;
	}

	fullpath = __realpath(path);
	if (!fullpath) {
		return -1; // errno already set
	}

	int ret = __is_dir(fullpath);
	if (ret < 0) {
		free(fullpath);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}

	strcpy(__cwd, fullpath);
	free(fullpath);
	return 0;
}