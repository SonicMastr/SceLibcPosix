#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
	DescriptorTranslation *fdmap = __fd_grab(s);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	int res = sceNetAccept(fdmap->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

	__fd_drop(fdmap);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	int s2 = __acquire_descriptor();

	if (s2 < 0) {
		errno = EMFILE;
		return -1;
	}

	__fdmap[s2]->sce_uid = res;
	__fdmap[s2]->type = VITA_DESCRIPTOR_SOCKET;
	return s2;
}