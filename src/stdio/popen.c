#include <errno.h>
#include <stdio.h>

FILE *popen(const char *cmd, const char *mode) {
	errno = ENOTSUP;
	return NULL;
}