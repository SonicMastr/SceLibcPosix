#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "vitafs.h"
#include "fios2.h"

int fcntl(int fd, int cmd, ...)
{
	unsigned long arg;
	va_list ap;
	va_start(ap, cmd);
	arg = va_arg(ap, unsigned long);
	va_end(ap);

	if (cmd == F_DUPFD) {
		if (arg < 0 || arg >= MAX_OPEN_FILES) {
			errno = EINVAL;
			return -1;
		}

		int dupfd = __duplicate_descriptor(fd, arg);
		if (dupfd < 0)
		{
			errno = EBADF;
			return -1;
		}

		errno = 0;
		return dupfd;
	}

	DescriptorTranslation *fdmap = __fd_grab(fd);
	if (fdmap == NULL)
	{
		errno = EBADF;
		return -1;
	}

	// The only existing flag is FD_CLOEXEC, and it is unsupported,
	// so F_GETFD always returns zero.
	if (cmd == F_GETFD)
	{
		__fd_drop(fdmap);
		return 0;
	}

	// Only net sockets are supported for F_GETFL/F_SETFL
	if (fdmap->type == VITA_DESCRIPTOR_SOCKET)
	{
		if (cmd == F_GETFL)
		{
			int optval = 0;
			unsigned int sce_optlen = sizeof(optval);
			int res = sceNetGetsockopt(fdmap->sce_uid, SOL_SOCKET, SO_NONBLOCK, &optval, &sce_optlen);

			__fd_drop(fdmap);
			if (res < 0)
			{
				errno = __scenet_errno_to_errno(res);
				return -1;
			}

			return (optval) ? O_NONBLOCK : 0;
		}

		if (cmd == F_SETFL)
		{
			int optval = (arg & O_NONBLOCK) ? 1 : 0;
			unsigned int sce_optlen = sizeof(optval);

			int optres = sceNetSetsockopt(fdmap->sce_uid, SOL_SOCKET, SO_NONBLOCK, &optval, &sce_optlen);

			__fd_drop(fdmap);
			if (optres < 0)
			{
				errno = __scenet_errno_to_errno(optres);
				return -1;
			}

			return 0;
		}
	}

	__fd_drop(fdmap);
	errno = ENOTSUP;
	return -1;
}