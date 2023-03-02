#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

ssize_t sendto(int s, const void *buf,
			   size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res = sceNetSendto(fdmap->sce_uid, buf, len, flags, (SceNetSockaddr *)dest_addr, (unsigned int)addrlen);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return res;
}