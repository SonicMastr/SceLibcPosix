#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int unlink(const char *path) {
	int ret;
	char *full_path = __realpath(path);
	if (!full_path) {
		return -1;
	}
	ret = sceFiosDeleteSync(NULL, full_path);
	if (ret < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
		return -1;
	}
	free(full_path);
	errno = 0;
	return 0;
}