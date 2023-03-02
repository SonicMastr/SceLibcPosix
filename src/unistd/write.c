#include <sys/unistd.h>
#include <errno.h>

#include <psp2/net/net.h>
#include <psp2/kernel/processmgr.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

ssize_t write(int fd, const void *buf, size_t nbytes) {
	int ret = 0;
	int type = ERROR_GENERIC;
	DescriptorTranslation *fdmap = __fd_grab(fd);
	if (!fdmap) {
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_TTY:
		ret = sceIoWrite(fdmap->sce_uid, buf, nbytes);
		break;
	case VITA_DESCRIPTOR_FILE:
		type = ERROR_FIOS;
		ret = sceFiosFHWriteSync(NULL, fdmap->sce_uid, buf, nbytes);
		break;
	case VITA_DESCRIPTOR_SOCKET:
		type = ERROR_SOCKET;
		ret = sceNetSend(fdmap->sce_uid, buf, nbytes, 0);
		break;
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = __make_sce_errno(EBADF);
		break;
	case VITA_DESCRIPTOR_PIPE:
	{
		size_t len = nbytes;
		if (len > 4 * 4096)
			len = 4 * 4096;
		ret = sceKernelSendMsgPipe(fdmap->sce_uid, buf, len, 1, NULL, NULL);
		if (ret == 0)
			ret = len;
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
