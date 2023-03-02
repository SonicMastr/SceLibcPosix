#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int truncate(const char *path, off_t length) {
	int ret;

	ret = sceFiosFileTruncateSync(NULL, path, length);

	if (ret < 0) {
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	errno = 0;
	return 0;
}
