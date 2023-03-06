#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// May not be entirely accurate
off_t ftello(FILE *f) {
	off_t pos = (off_t)ftell(f);
	if (pos > LONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}
	return pos;
}