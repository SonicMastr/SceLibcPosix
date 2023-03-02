#include <errno.h>

int readlink(const char *path, char *buf, size_t bufsize) {
	errno = ENOSYS;
	return -1;
}
