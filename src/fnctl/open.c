#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitafs.h"
#include "fios2.h"

int open(const char *filename, int flags, ...) {
	int ret;
	int sce_flags = _fcntl2sony(flags);
	int is_dir = 0;

	// check flags

	char *full_path = __realpath(filename);
	if (!full_path) {
		return -1;
	}

	// get full path and stat. if dir - use dir funcs, otherwise - filename
	// if O_DIRECTORY passed - check that path is indeed dir and return ENOTDIR otherwise
	// ENOENT, etc is handled by sce funcs
	if (__is_dir(full_path) == 0) {
		is_dir = 1;
	}
	if (flags & O_DIRECTORY && !is_dir) {
		free(full_path);
		errno = ENOTDIR;
		return -1;
	}

	if (is_dir) {
		SceFiosDH handle = 0;
		SceFiosBuffer buf = SCE_FIOS_BUFFER_INITIALIZER;
		ret = sceFiosDHOpenSync(NULL, &handle, full_path, buf);
		ret = ret < 0 ? ret : handle;
	} else {
		SceFiosFH handle = 0;
		SceFiosOpenParams openParams = SCE_FIOS_OPENPARAMS_INITIALIZER;
		openParams.openFlags = sce_flags;
		ret = sceFiosFHOpenSync(NULL, &handle, full_path, &openParams);
		ret = ret < 0 ? ret : handle;
	}

	if (ret < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_GENERIC);
		return -1;
	}

	int fd = __acquire_descriptor();

	if (fd < 0) {
		free(full_path);
		is_dir ? sceFiosDHCloseSync(NULL, ret) : sceFiosFHCloseSync(NULL, ret);
		errno = EMFILE;
		return -1;
	}

	__fdmap[fd]->sce_uid = ret;
	__fdmap[fd]->type = is_dir ? VITA_DESCRIPTOR_DIRECTORY : VITA_DESCRIPTOR_FILE;

	free(full_path);

	errno = 0;
	return fd;
}