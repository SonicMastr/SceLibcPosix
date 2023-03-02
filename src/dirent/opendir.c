#include <sys/dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

#include "vitadescriptor.h"
#include "vitaerror.h"

DIR *opendir(const char *dirname) {
	int fd;

	if ((fd = open(dirname, O_RDONLY | O_DIRECTORY)) == -1)
		return (NULL);
	DIR *ret = __opendir_common(fd);
	if (!ret) {
		close(fd);
	}
	return ret;
}