#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

int lstat(const char *path, struct stat *buf) {
	return stat(path, buf);
}