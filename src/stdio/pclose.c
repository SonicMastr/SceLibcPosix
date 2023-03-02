#include <errno.h>
#include <stdio.h>

int pclose(FILE *f) {
	errno = ENOTSUP;
	return -1;
}