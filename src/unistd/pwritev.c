#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include <psp2/net/net.h>
#include <psp2/kernel/processmgr.h>

#include <sys/param.h>
#include <sys/uio.h>

#include "vitadescriptor.h"
#include "vitaerror.h"
#include "fios2.h"

#ifndef SSIZE_MAX
/* ssize_t is not formally required to be the signed type
	 corresponding to size_t, but it is for all configurations supported
	 by glibc.  */
#if __WORDSIZE == 64 || __WORDSIZE32_SIZE_ULONG
#define SSIZE_MAX LONG_MAX
#else
#define SSIZE_MAX INT_MAX
#endif
#endif

static void ifree(char **ptrp) {
	free(*ptrp);
}

hidden ssize_t __pwritev(int fd, const struct iovec *vector, size_t count, off_t offset) {
    /* Find the total number of bytes to be written.  */
	size_t bytes = 0;
	for (int i = 0; i < count; ++i) {
		/* Check for ssize_t overflow.  */
		if (SSIZE_MAX - bytes < vector[i].iov_len) {
			return -1;
		}
		bytes += vector[i].iov_len;
	}

	char *buffer;
	char *malloced_buffer __attribute__((__cleanup__(ifree))) = NULL;

	malloced_buffer = buffer = (char *)malloc(bytes);
	if (buffer == NULL)
		/* XXX I don't know whether it is acceptable to try writing the data in chunks.  Probably not so we just fail here.  */
		return -1;

	/* Copy the data into BUFFER.  */
	size_t to_copy = bytes;
	char *bp = buffer;
	for (int i = 0; i < count; ++i) {
		size_t copy = MIN(vector[i].iov_len, to_copy);

		bp = mempcpy((void *)bp, (void *)vector[i].iov_base, copy);

		to_copy -= copy;
		if (to_copy == 0)
			break;
	}

	ssize_t bytes_written = pwrite(fd, buffer, bytes, offset);

	return bytes_written;
}

ssize_t pwritev(int fd, const struct iovec *iov, size_t count, off_t off) {
    int ret;
    int type = ERROR_GENERIC;
	DescriptorTranslation *fdmap = __fd_grab(fd);

	if (!fdmap) {
		errno = EBADF;
		return -1;
	}

	switch (fdmap->type) {
	case VITA_DESCRIPTOR_DIRECTORY:
		ret = __make_sce_errno(EBADF);
		break;
	case VITA_DESCRIPTOR_FILE:
        type = ERROR_FIOS;
		ret = sceFiosFHPwritevSync(NULL, fdmap->sce_uid, iov, count, off);
		break;
	case VITA_DESCRIPTOR_TTY:
	case VITA_DESCRIPTOR_SOCKET:
	case VITA_DESCRIPTOR_PIPE:
		ret = __pwritev(fdmap->sce_uid, iov, count, off);
		break;
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
