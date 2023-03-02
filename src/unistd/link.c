#include <errno.h>

int link(const char *existing, const char *new) {
	errno = ENOSYS;
	return -1;
}