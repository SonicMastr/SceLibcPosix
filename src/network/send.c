#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

ssize_t send(int s, const void *buf, size_t len, int flags) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res = sceNetSend(fdmap->sce_uid, buf, len, flags);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return res;
}