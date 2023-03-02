#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int rmdir(const char *pathname) {
	int ret;
	char *full_path = __realpath(pathname);
	if (!full_path) {
		return -1;
	}

	if ((ret = sceFiosDirectoryDeleteSync(NULL, full_path)) < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}
	free(full_path);
	errno = 0;
	return 0;
}