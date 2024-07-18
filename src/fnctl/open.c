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

	unsigned long arg;
	va_list ap;
	va_start(ap, flags);
	arg = va_arg(ap, unsigned long);
	va_end(ap);

	char *full_path = __realpath(filename);
	if (!full_path) {
		return -1;
	}

	// get full path and stat. if dir - use dir funcs, otherwise - filename
	// if O_DIRECTORY passed - check that path is indeed dir
	if (__is_dir(full_path) == 0) {
		is_dir = 1;
	}

	if (is_dir && (flags & O_DIRECTORY)) { // This is so damn ugly
		SceFiosDH handle = 0;
		SceFiosBuffer buf = SCE_FIOS_BUFFER_INITIALIZER;
		if (flags & O_CREAT) {
			if ((ret = sceFiosDirectoryCreateSync(NULL, full_path)) < 0) {
				errno = __sce_errno_to_errno(ret, ERROR_FIOS);
				if (errno != EEXIST) {
					free(full_path);
					return -1;
				}
			}
		}
		ret = sceFiosDHOpenSync(NULL, &handle, full_path, buf);
		ret = ret < 0 ? ret : handle;
	} else {
		SceFiosFH handle = 0;
		SceFiosOpenParams openParams = SCE_FIOS_OPENPARAMS_INITIALIZER;
		openParams.openFlags = sce_flags;
		if ((flags & O_CREAT) && arg)
			ret = sceFiosFHOpenWithModeSync(NULL, &handle, full_path, &openParams, arg);
		else
			ret = sceFiosFHOpenSync(NULL, &handle, full_path, &openParams);
		ret = ret < 0 ? ret : handle;
	}

	if (ret < 0) {
		free(full_path);
		errno = __sce_errno_to_errno(ret, ERROR_FIOS);
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