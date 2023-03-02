#include <sys/unistd.h>

int fdatasync(int fd) {
	return fsync(fd);
}
