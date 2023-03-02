#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

void seekdir(DIR *dirp, long int index) {
	if (!dirp) {
		errno = EBADF;
		return;
	}

	if (index < dirp->index)
		rewinddir(dirp);

	if (index < dirp->index) {
		return;
	}

	while (index != dirp->index) {
		if (!readdir(dirp)) {
			errno = ENOENT;
			return;
		}
	}
}