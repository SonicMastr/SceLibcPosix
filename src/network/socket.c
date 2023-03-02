#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitanet.h"

int socket(int domain, int type, int protocol) {
	_net_init();
	int res = sceNetSocket("", domain, type, protocol);

	if (res < 0) {
		errno = __scenet_errno_to_errno(res);
		return -1;
	}

	int s = __acquire_descriptor();

	if (s < 0) {
		errno = EMFILE;
		return -1;
	}

	__fdmap[s]->sce_uid = res;
	__fdmap[s]->type = VITA_DESCRIPTOR_SOCKET;
	return s;
}
