/*
Copyright (C) 2017, David "Davee" Morgan
Copyright (C) 2018, Sunguk Lee
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#include <errno.h>

#include <psp2/net/net.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "fios2.h"

int __sce_errno_to_errno(int sce_errno, int type) {
	if (type == ERROR_FIOS) {
		return __scefios2_errno_to_errno(sce_errno);
	}
	if (type == ERROR_SOCKET) {
		return __scenet_errno_to_errno(sce_errno);
	}
	return sce_errno & SCE_ERRNO_MASK;
}

int __make_sce_errno(int posix_errno) {
	return SCE_ERRNO_NONE | posix_errno;
}

// From SceLibc
int __scefios2_errno_to_errno(int sce_errno) {
	switch (sce_errno) {
	case SCE_FIOS_ERROR_ACCESS:
		return EPERM;
	case SCE_FIOS_ERROR_BAD_PATH:
		return ENOENT;
	case SCE_FIOS_ERROR_UNIMPLEMENTED:
	case SCE_FIOS_ERROR_CANT_ALLOCATE_OP:
	case SCE_FIOS_ERROR_CANT_ALLOCATE_FH:
	case SCE_FIOS_ERROR_CANT_ALLOCATE_DH:
	case SCE_FIOS_ERROR_CANT_ALLOCATE_CHUNK:
	case SCE_FIOS_ERROR_BAD_ARCHIVE:
		return EIO;
	case SCE_FIOS_ERROR_BAD_FH:
	case SCE_FIOS_ERROR_BAD_DH:
		return EBADF;
	case SCE_FIOS_ERROR_NOT_A_FILE:
		return EISDIR;
	case SCE_FIOS_ERROR_NOT_A_DIRECTORY:
		return ENOTDIR;
	case SCE_FIOS_ERROR_READ_ONLY:
		return EROFS;
	case SCE_FIOS_ERROR_EOF:
		return ERANGE;
	case SCE_FIOS_ERROR_MEDIA_GONE:
		return ENODEV;
	case SCE_FIOS_ERROR_BUSY:
		return EBUSY;
	case SCE_FIOS_ERROR_ALREADY_EXISTS:
		return EEXIST;
	case SCE_FIOS_ERROR_CANCELLED:
		return EINTR;
	case SCE_FIOS_ERROR_PATH_TOO_LONG:
		return ENAMETOOLONG;
	case SCE_FIOS_ERROR_BAD_PTR:
	case SCE_FIOS_ERROR_BAD_OFFSET:
	case SCE_FIOS_ERROR_BAD_SIZE:
	case SCE_FIOS_ERROR_BAD_IOVCNT:
	case SCE_FIOS_ERROR_BAD_ALIGNMENT:
	case SCE_FIOS_ERROR_BAD_FLAGS:
	case SCE_FIOS_ERROR_BAD_RESOLVE_TYPE:
		return EINVAL;
	default:
		return sce_errno;
	}
}

int __scenet_errno_to_errno(int sce_errno) {
	switch (sce_errno & SCE_ERRNO_MASK) {
	case SCE_NET_EPERM:
		return EPERM;
	case SCE_NET_ENOENT:
		return ENOENT;
	case SCE_NET_ESRCH:
		return ESRCH;
	case SCE_NET_EINTR:
		return EINTR;
	case SCE_NET_EIO:
		return EIO;
	case SCE_NET_ENXIO:
		return ENXIO;
	case SCE_NET_E2BIG:
		return E2BIG;
	case SCE_NET_ENOEXEC:
		return ENOEXEC;
	case SCE_NET_EBADF:
		return EBADF;
	case SCE_NET_ECHILD:
		return ECHILD;
	case SCE_NET_EDEADLK:
		return EDEADLK;
	case SCE_NET_ENOMEM:
		return ENOMEM;
	case SCE_NET_EACCES:
		return EACCES;
	case SCE_NET_EFAULT:
		return EFAULT;
	case SCE_NET_ENOTBLK:
		return ENOTBLK;
	case SCE_NET_EBUSY:
		return EBUSY;
	case SCE_NET_EEXIST:
		return EEXIST;
	case SCE_NET_EXDEV:
		return EXDEV;
	case SCE_NET_ENODEV:
		return ENODEV;
	case SCE_NET_ENOTDIR:
		return ENOTDIR;
	case SCE_NET_EISDIR:
		return EISDIR;
	case SCE_NET_EINVAL:
		return EINVAL;
	case SCE_NET_ENFILE:
		return ENFILE;
	case SCE_NET_EMFILE:
		return EMFILE;
	case SCE_NET_ENOTTY:
		return ENOTTY;
	case SCE_NET_ETXTBSY:
		return ETXTBSY;
	case SCE_NET_EFBIG:
		return EFBIG;
	case SCE_NET_ENOSPC:
		return ENOSPC;
	case SCE_NET_ESPIPE:
		return ESPIPE;
	case SCE_NET_EROFS:
		return EROFS;
	case SCE_NET_EMLINK:
		return EMLINK;
	case SCE_NET_EPIPE:
		return EPIPE;
	case SCE_NET_EDOM:
		return EDOM;
	case SCE_NET_ERANGE:
		return ERANGE;
	case SCE_NET_EAGAIN:
		return EAGAIN;
		// these values are the same as EAGAIN
		//		case SCE_NET_EWOULDBLOCK:
		//			return EWOULDBLOCK;
	case SCE_NET_EINPROGRESS:
		return EINPROGRESS;
	case SCE_NET_EALREADY:
		return EALREADY;
	case SCE_NET_ENOTSOCK:
		return ENOTSOCK;
	case SCE_NET_EDESTADDRREQ:
		return EDESTADDRREQ;
	case SCE_NET_EMSGSIZE:
		return EMSGSIZE;
	case SCE_NET_EPROTOTYPE:
		return EPROTOTYPE;
	case SCE_NET_ENOPROTOOPT:
		return ENOPROTOOPT;
	case SCE_NET_EPROTONOSUPPORT:
		return EPROTONOSUPPORT;
	case SCE_NET_ESOCKTNOSUPPORT:
		return ESOCKTNOSUPPORT;
	case SCE_NET_EOPNOTSUPP:
		return EOPNOTSUPP;
	case SCE_NET_EPFNOSUPPORT:
		return EPFNOSUPPORT;
	case SCE_NET_EAFNOSUPPORT:
		return EAFNOSUPPORT;
	case SCE_NET_EADDRINUSE:
		return EADDRINUSE;
	case SCE_NET_EADDRNOTAVAIL:
		return EADDRNOTAVAIL;
	case SCE_NET_ENETDOWN:
		return ENETDOWN;
	case SCE_NET_ENETUNREACH:
		return ENETUNREACH;
	case SCE_NET_ENETRESET:
		return ENETRESET;
	case SCE_NET_ECONNABORTED:
		return ECONNABORTED;
	case SCE_NET_ECONNRESET:
		return ECONNRESET;
	case SCE_NET_ENOBUFS:
		return ENOBUFS;
	case SCE_NET_EISCONN:
		return EISCONN;
	case SCE_NET_ENOTCONN:
		return ENOTCONN;
	case SCE_NET_ESHUTDOWN:
		return ESHUTDOWN;
	case SCE_NET_ETOOMANYREFS:
		return ETOOMANYREFS;
	case SCE_NET_ETIMEDOUT:
		return ETIMEDOUT;
	case SCE_NET_ECONNREFUSED:
		return ECONNREFUSED;
	case SCE_NET_ELOOP:
		return ELOOP;
	case SCE_NET_ENAMETOOLONG:
		return ENAMETOOLONG;
	case SCE_NET_EHOSTDOWN:
		return EHOSTDOWN;
	case SCE_NET_EHOSTUNREACH:
		return EHOSTUNREACH;
	case SCE_NET_ENOTEMPTY:
		return ENOTEMPTY;
	case SCE_NET_EUSERS:
		return EUSERS;
	case SCE_NET_EDQUOT:
		return EDQUOT;
	case SCE_NET_ESTALE:
		return ESTALE;
	case SCE_NET_EREMOTE:
		return EREMOTE;
	case SCE_NET_ENOLCK:
		return ENOLCK;
	case SCE_NET_ENOSYS:
		return ENOSYS;
	case SCE_NET_EIDRM:
		return EIDRM;
	case SCE_NET_EOVERFLOW:
		return EOVERFLOW;
	case SCE_NET_EILSEQ:
		return EILSEQ;
	case SCE_NET_ENOTSUP:
		return ENOTSUP;
	case SCE_NET_ECANCELED:
		return ECANCELED;
	case SCE_NET_EBADMSG:
		return EBADMSG;
	case SCE_NET_ENODATA:
		return ENODATA;
	case SCE_NET_ENOSR:
		return ENOSR;
	case SCE_NET_ENOSTR:
		return ENOSTR;
	case SCE_NET_ETIME:
		return ETIME;
	default:
		return sce_errno;
	}
}