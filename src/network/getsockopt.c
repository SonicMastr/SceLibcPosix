#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res;
	if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
	{
		if (optlen < sizeof(struct timeval))
		{
			__fd_drop(fdmap);
			errno = EINVAL;
			return -1;
		}

		int wait = 0;
		unsigned int sce_optlen = sizeof(wait);
		res = sceNetGetsockopt(fdmap->sce_uid, level, optname, &wait, &sce_optlen);

		*optlen = sizeof(struct timeval);
		struct timeval *timeout = optval;
		timeout->tv_sec = wait / 1000000;
		timeout->tv_usec = wait % 1000000;
	} else {
		res = sceNetGetsockopt(fdmap->sce_uid, level, optname, optval, (unsigned int *)optlen);
	}

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	return 0;
}