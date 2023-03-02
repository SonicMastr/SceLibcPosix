#include <sys/unistd.h>
#include <errno.h>

#include <psp2/net/net.h>
#include <psp2/kernel/processmgr.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

ssize_t read(int fd, void *ptr, size_t len) {
	int ret;
	int type = ERROR_GENERIC;
	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
		ret = sceIoRead(fdmap->sce_uid, ptr, len);
		break;
	case VITA_DESCRIPTOR_FILE:
		type = ERROR_FIOS;
		ret = sceFiosFHReadSync(NULL, fdmap->sce_uid, ptr, len);
		break;
	case VITA_DESCRIPTOR_SOCKET:
		type = ERROR_SOCKET;
		ret = sceNetRecv(fdmap->sce_uid, ptr, len, 0);
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = __make_sce_errno(EBADF);
		break;
	case VITA_DESCRIPTOR_PIPE:
	{
		size_t rlen = len;
		if (rlen > 4 * 4096)
			rlen = 4 * 4096;
		ret = sceKernelReceiveMsgPipe(fdmap->sce_uid, ptr, rlen, 1, NULL, NULL);
		if (ret == 0)
			ret = rlen;
		break;
	}
	}

	__fd_drop(fdmap);

	if (ret < 0) {
		if (ret != -1)
			errno = __sce_errno_to_errno(ret, type);
		return -1;
	}

	errno = 0;
	return ret;
}