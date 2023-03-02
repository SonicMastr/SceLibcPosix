#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res;
	if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
		if (optlen < sizeof(struct timeval)) {
			__fd_drop(fdmap);
			errno = EINVAL;
			return -1;
		}
		const struct timeval *timeout = optval;
		int wait = timeout->tv_sec * 1000000 + timeout->tv_usec;
		res = sceNetSetsockopt(fdmap->sce_uid, level, optname, &wait, sizeof(wait));
	} else {
		res = sceNetSetsockopt(fdmap->sce_uid, level, optname, optval, optlen);
	}

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return 0;
}