#include <errno.h>
#include <unistd.h>

#include <stdlib.h>

#include "vitaerror.h"
#include "vitadescriptor.h"

int isatty(int fd) {
	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return 0;
	}

	int istty = (fdmap->type == VITA_DESCRIPTOR_TTY);

	__fd_drop(fdmap);
	return istty;
}