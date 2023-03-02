#include <errno.h>
#include <sys/unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

char *getcwd(char *buf, size_t size) {
	__init_cwd();

	if (buf != NULL && size == 0) {
		errno = EINVAL;
		return NULL;
	}

	if (buf && strlen(__cwd) >= size) {
		errno = ERANGE;
		return NULL;
	}

	// latest POSIX says it's implementation-defined
	// musl, glibc and bionic allocs buffer if it's null.
	// so we'll do the same
	if (buf == NULL) {
		buf = (char *)calloc(PATH_MAX, sizeof(char));
		if (buf == NULL) {
			errno = ENOMEM;
			return NULL;
		}
	}

	strcpy(buf, __cwd);
	return buf;
}