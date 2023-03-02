#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int chown(const char *path, uid_t owner, gid_t group) {
	// Implementation note: there's no real chown on vita
	// We only check for path correctness
	struct SceFiosStat stat = { 0 };
	char *full_path = __realpath(path);
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

	free(full_path);
	return 0;
}