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
		if (0 > sceFiosFHTell(fd)) { // C++ is a bit weird
			errno = EBADF;
			return -1;
		}
		type = ERROR_FIOS;
		ret = sceFiosFHReadSync(NULL, fd, ptr, len);
	} else {
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
			size_t p_res = 0;
			ret = sceKernelReceiveMsgPipe(fdmap->sce_uid, ptr, rlen, 1, &p_res, NULL);
			if (ret == 0) 
				ret = p_res;
			break;
		}
		}

		__fd_drop(fdmap);
	}

	if (ret < 0) {
		if (ret != -1)
			errno = __sce_errno_to_errno(ret, type);
		return -1;
	}

	errno = 0;
	return ret;
}