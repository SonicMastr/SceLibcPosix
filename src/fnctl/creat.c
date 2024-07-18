#include <fcntl.h>

int creat(const char *filename, mode_t mode) {
	// Mode is going to get tossed out anyway on this platform using FIOS2
	return open(filename, O_CREAT | O_WRONLY | O_TRUNC, mode);
}