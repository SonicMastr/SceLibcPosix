#include <reent.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syslimits.h>
#include <limits.h>
#include <string.h>

#include <psp2/types.h>
#include <psp2/io/stat.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/devctl.h>
#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"
#include "fios2.h"

#define MAX_PATH_LENGTH 256

static char __cwd[PATH_MAX] = { 0 };

// Fios2 init vars
int64_t g_OpStorage[SCE_FIOS_OP_STORAGE_SIZE(64, MAX_PATH_LENGTH) / sizeof(int64_t) + 1];
int64_t g_ChunkStorage[SCE_FIOS_CHUNK_STORAGE_SIZE(1024) / sizeof(int64_t) + 1];
int64_t g_FHStorage[SCE_FIOS_FH_STORAGE_SIZE(32, MAX_PATH_LENGTH) / sizeof(int64_t) + 1];
int64_t g_DHStorage[SCE_FIOS_DH_STORAGE_SIZE(8, MAX_PATH_LENGTH) / sizeof(int64_t) + 1];

void __init_fios2() {
	SceFiosParams params = SCE_FIOS_PARAMS_INITIALIZER;

	/*E Provide required storage buffers. */
	params.opStorage.pPtr = g_OpStorage;
	params.opStorage.length = sizeof(g_OpStorage);
	params.chunkStorage.pPtr = g_ChunkStorage;
	params.chunkStorage.length = sizeof(g_ChunkStorage);
	params.fhStorage.pPtr = g_FHStorage;
	params.fhStorage.length = sizeof(g_FHStorage);
	params.dhStorage.pPtr = g_DHStorage;
	params.dhStorage.length = sizeof(g_DHStorage);

	params.pathMax = MAX_PATH_LENGTH;

	params.pMemcpy = memcpy;

	sceFiosInitialize(&params);
}

// get end of drive position from full path
int __get_drive(const char *path) {
	int i;
	for (i = 0; path[i]; i++) {
		if (!((path[i] >= 'a' && path[i] <= 'z') || (path[i] >= '0' && path[i] <= '9')))
			break;
	}

	if (path[i] == ':')
		return i + 1;
	return 0;
}

void __init_cwd() {
	if (strlen(__cwd) == 0) {
		// init cwd
		strcpy(__cwd, "app0:");
	}
}

// modified from bionic
char *__resolve_path(const char *path, char resolved[PATH_MAX]) {
	char *p, *q, *s;
	size_t left_len, resolved_len;
	char left[PATH_MAX], next_token[PATH_MAX];
	if (path[0] == '/') {
		resolved[0] = '/';
		resolved[1] = '\0';
		if (path[1] == '\0')
			return (resolved);
		resolved_len = 1;
		left_len = strlcpy(left, path + 1, sizeof(left));
	} else {
		resolved_len = 0;
		left_len = strlcpy(left, path, sizeof(left));
	}
	if (left_len >= sizeof(left) || resolved_len >= PATH_MAX) {
		errno = ENAMETOOLONG;
		return (NULL);
	}
	/*
	 * Iterate over path components in `left'.
	 */
	while (left_len != 0) {
		/*
		 * Extract the next path component and adjust `left'
		 * and its length.
		 */
		p = strchr(left, '/');
		s = p ? p : left + left_len;
		if (s - left >= sizeof(next_token)) {
			errno = ENAMETOOLONG;
			return (NULL);
		}
		memcpy(next_token, left, s - left);
		next_token[s - left] = '\0';
		left_len -= s - left;
		if (p != NULL)
			memmove(left, s + 1, left_len + 1);
		if (resolved[resolved_len - 1] != '/') {
			if (resolved_len + 1 >= PATH_MAX) {
				errno = ENAMETOOLONG;
				return (NULL);
			}
			resolved[resolved_len++] = '/';
			resolved[resolved_len] = '\0';
		}
		if (next_token[0] == '\0')
			continue;
		else if (strcmp(next_token, ".") == 0)
			continue;
		else if (strcmp(next_token, "..") == 0) {
			/*
			 * Strip the last path component except when we have
			 * single "/"
			 */
			if (resolved_len > 1) {
				resolved[resolved_len - 1] = '\0';
				q = strrchr(resolved, '/') + 1;
				*q = '\0';
				resolved_len = q - resolved;
			}
			continue;
		}
		/*
		 * Append the next path component.
		 */
		resolved_len = strlcat(resolved, next_token, PATH_MAX);
		if (resolved_len >= PATH_MAX) {
			errno = ENAMETOOLONG;
			return (NULL);
		}
	}
	/*
	 * Remove trailing slash except when the resolved pathname
	 * is a single "/".
	 */
	if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
		resolved[resolved_len - 1] = '\0';
	return (resolved);
}

// internal, without stat check. Used for mkdir/open/etc.
char *__realpath(const char *path) {
	char resolved[PATH_MAX] = { 0 };
	char result[PATH_MAX] = { 0 };
	char *resolved_path = NULL;

	// can't have null path
	if (!path) {
		errno = EINVAL;
		return NULL;
	}

	// empty path would resolve to cwd
	// POSIX.1-2008 states that ENOENT should be returned if path points to empty string
	if (strlen(path) == 0) {
		errno = ENOENT;
		return NULL;
	}

	resolved_path = (char *)calloc(PATH_MAX, sizeof(char));
	if (!resolved_path) {
		errno = ENOMEM;
		return NULL;
	}

	int d = __get_drive(path);

	if (d) // absolute path with drive
	{
		__resolve_path(path + d, resolved);
		if (strlen(resolved) + d < PATH_MAX) {
			strncpy(result, path, d);
			strcat(result, resolved);
			strcpy(resolved_path, result);
			return resolved_path;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	} else if (path[0] == '/') // absolute path without drive
	{
		__init_cwd();
		__resolve_path(path, resolved);
		d = __get_drive(__cwd);
		if (strlen(resolved) + d < PATH_MAX) {
			strncpy(result, __cwd, d);
			strcat(result, resolved);
			strcpy(resolved_path, result);
			return resolved_path;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	} else // relative path
	{
		__init_cwd();
		char full_path[PATH_MAX] = { 0 };
		if (strlen(__cwd) + strlen(path) < PATH_MAX) {
			strcpy(full_path, __cwd);
			strcat(full_path, "/");
			strcat(full_path, path);
			d = __get_drive(full_path);

			__resolve_path(full_path + d, resolved);
			if (strlen(resolved) + d < PATH_MAX) {
				strncpy(result, full_path, d);
				strcat(result, resolved);
				strcpy(resolved_path, result);
				return resolved_path;
			}
			errno = ENAMETOOLONG;
			free(resolved_path);
			return NULL;
		}
		errno = ENAMETOOLONG;
		free(resolved_path);
		return NULL;
	}
}

int __is_dir(const char *path) {
	SceFiosStat stat;
	int ret = sceFiosStatSync(NULL, path, &stat);
	if (ret < 0) {
		return ret;
	}
	if (!SCE_S_ISDIR(stat.st_mode)) {
		return __make_sce_errno(ENOTDIR);
	}
	return 0;
}

int _fcntl2sony(int flags) {
	int out = 0;
	if (flags & O_RDWR)
		out |= SCE_FIOS_O_RDWR;
	else if (flags & O_WRONLY)
		out |= SCE_FIOS_O_WRONLY;
	else
		out |= SCE_FIOS_O_RDONLY;
	if (flags & O_NONBLOCK)
		out |= SCE_O_NBLOCK;
	if (flags & O_APPEND)
		out |= SCE_FIOS_O_APPEND;
	if (flags & O_CREAT)
		out |= SCE_FIOS_O_CREAT;
	if (flags & O_TRUNC)
		out |= SCE_FIOS_O_TRUNC;
	if (flags & O_EXCL)
		out |= SCE_O_EXCL;
	return out;
}
