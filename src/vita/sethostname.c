#include <errno.h>

int sethostname(const char *name, size_t len) {
	errno = EPERM;
	return -1;
}