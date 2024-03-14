#include <fcntl.h>
#include <sys/unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <reent.h>

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/net.h>

#include "vitadescriptor.h"
#include "vitaerror.h"

#include "fios2.h"

extern FILE _Stdin, _Stdout, _Stderr;

DescriptorTranslation *__fdmap[MAX_OPEN_FILES];
DescriptorTranslation __fdmap_pool[MAX_OPEN_FILES];

SceKernelLwMutexWork _fd_mutex __attribute__((aligned(8)));

void _init_fd(void) {
	int ret;
	sceKernelCreateLwMutex(&_fd_mutex, "fd storage mutex", SCE_KERNEL_MUTEX_ATTR_RECURSIVE, 1, NULL);

	memset(__fdmap, 0, sizeof(__fdmap));
	memset(__fdmap_pool, 0, sizeof(__fdmap_pool));

	__fdmap[STDIN_FILENO] = &__fdmap_pool[STDIN_FILENO];
	__fdmap[STDIN_FILENO]->sce_uid = sceKernelGetStdin();
	__fdmap[STDIN_FILENO]->type = VITA_DESCRIPTOR_TTY;
	__fdmap[STDIN_FILENO]->ref_count = 1;

	__fdmap[STDOUT_FILENO] = &__fdmap_pool[STDOUT_FILENO];
	__fdmap[STDOUT_FILENO]->sce_uid = sceKernelGetStdout();
	__fdmap[STDOUT_FILENO]->type = VITA_DESCRIPTOR_TTY;
	__fdmap[STDOUT_FILENO]->ref_count = 1;

	__fdmap[STDERR_FILENO] = &__fdmap_pool[STDERR_FILENO];
	__fdmap[STDERR_FILENO]->sce_uid = sceKernelGetStderr();
	__fdmap[STDERR_FILENO]->type = VITA_DESCRIPTOR_TTY;
	__fdmap[STDERR_FILENO]->ref_count = 1;

	if (_global_impure_ptr->__sdidinit)
    {
		sceKernelUnlockLwMutex(&_fd_mutex, 1);
		return;
	}

	_global_impure_ptr->_stdin = &_Stdin;
	_global_impure_ptr->_stdout = &_Stdout;
	_global_impure_ptr->_stderr = &_Stderr;

	_global_impure_ptr->__sdidinit = 1;

	sceKernelUnlockLwMutex(&_fd_mutex, 1);
}

void _free_fd(void) {
	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	if (__fdmap[STDIN_FILENO]) {
		memset(__fdmap[STDIN_FILENO], 0, sizeof(DescriptorTranslation));
		__fdmap[STDIN_FILENO] = NULL;
	}
	if (__fdmap[STDOUT_FILENO]) {
		memset(__fdmap[STDOUT_FILENO], 0, sizeof(DescriptorTranslation));
		__fdmap[STDOUT_FILENO] = NULL;
	}
	if (__fdmap[STDERR_FILENO]) {
		memset(__fdmap[STDERR_FILENO], 0, sizeof(DescriptorTranslation));
		__fdmap[STDERR_FILENO] = NULL;
	}

	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	sceKernelDeleteLwMutex(&_fd_mutex);
}

int __acquire_descriptor(void) {
	int fd = -1;
	int i = 0;
	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	// get free descriptor
	for (fd = 3; fd < MAX_OPEN_FILES; ++fd) {
		if (__fdmap[fd] == NULL) {
			// get free pool
			for (i = 0; i < MAX_OPEN_FILES; ++i) {
				if (__fdmap_pool[i].ref_count == 0) {
					__fdmap[fd] = &__fdmap_pool[i];
					__fdmap[fd]->ref_count = 1;
					sceKernelUnlockLwMutex(&_fd_mutex, 1);
					return fd;
				}
			}
		}
	}

	// no mores descriptors available...
	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return -1;
}

int __release_descriptor(int fd) {
	DescriptorTranslation *map = NULL;
	int res = -1;

	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	if (is_fd_valid(fd) && __fd_drop(__fdmap[fd]) >= 0) {
		__fdmap[fd] = NULL;
		res = 0;
	}

	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return res;
}

int __duplicate_descriptor(int fd) {
	int fd2 = -1;

	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	if (is_fd_valid(fd)) {
		// get free descriptor
		for (fd2 = 3; fd2 < MAX_OPEN_FILES; ++fd2) {
			if (__fdmap[fd2] == NULL) {
				__fdmap[fd2] = __fdmap[fd];
				__fdmap[fd2]->ref_count++;
				sceKernelUnlockLwMutex(&_fd_mutex, 1);
				return fd2;
			}
		}
	}

	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return -1;
}

int __descriptor_ref_count(int fd) {
	int res = 0;
	sceKernelLockLwMutex(&_fd_mutex, 1, 0);
	res = __fdmap[fd]->ref_count;
	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return res;
}

DescriptorTranslation *__fd_grab(int fd) {
	DescriptorTranslation *map = NULL;

	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	if (is_fd_valid(fd)) {
		map = __fdmap[fd];

		if (map)
			map->ref_count++;
	}
	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return map;
}

int __fd_drop(DescriptorTranslation *map) {
	sceKernelLockLwMutex(&_fd_mutex, 1, 0);

	if (map->ref_count == 1) {
		int ret = 0;

		switch (map->type) {
		case VITA_DESCRIPTOR_TTY:
		{
			ret = sceIoClose(map->sce_uid);
			break;
		}
		case VITA_DESCRIPTOR_FILE:
		{
			ret = sceFiosFHCloseSync(NULL, map->sce_uid);
			break;
		}
		case VITA_DESCRIPTOR_DIRECTORY:
		{
			ret = sceFiosDHCloseSync(NULL, map->sce_uid);
			break;
		}
		case VITA_DESCRIPTOR_SOCKET:
		{
			ret = sceNetSocketClose(map->sce_uid);
			if (ret < 0) {
				sceKernelUnlockLwMutex(&_fd_mutex, 1);
				return -__scenet_errno_to_errno(ret);
			}
			break;
		}
		case VITA_DESCRIPTOR_PIPE:
		{
			ret = sceKernelDeleteMsgPipe(map->sce_uid);
			break;
		}
		}

		if (ret < 0) {
			sceKernelUnlockLwMutex(&_fd_mutex, 1);
			return -__sce_errno_to_errno(ret, ERROR_GENERIC);
		}

		map->ref_count--;
		memset(map, 0, sizeof(DescriptorTranslation));
	} else {
		map->ref_count--;
	}

	sceKernelUnlockLwMutex(&_fd_mutex, 1);
	return 0;
}