#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int futimens(int fd, const struct timespec times[2]) {
	return utimensat(fd, 0, times, 0);
}
