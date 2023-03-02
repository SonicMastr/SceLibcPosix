#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int shutdown(int s, int how) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res = sceNetShutdown(fdmap->sce_uid, how);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return 0;
}