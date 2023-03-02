#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int listen(int s, int backlog) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	// Vita's Berkeley sockets implementation rejects a backlog of 0.
	// However, most other OSes allow this, so force it to 1 for compat.
	if (backlog < 1)
		backlog = 1;

	int res = sceNetListen(fdmap->sce_uid, backlog);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return 0;
}