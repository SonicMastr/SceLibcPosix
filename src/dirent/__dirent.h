#ifndef __DIRENT_H
#define __DIRENT_H

#include "features.h"
#include <sys/dirent.h>

struct DIR_
{
	int fd;
	struct dirent dir;
	int index;
};

hidden DIR *__opendir_common(int fd);

#endif
