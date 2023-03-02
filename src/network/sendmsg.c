#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res = sceNetSendmsg(fdmap->sce_uid, (const SceNetMsghdr *)msg, flags);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return res;
}