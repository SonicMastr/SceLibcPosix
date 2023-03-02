#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "__dirent.h"

int readdir_r(DIR *restrict dirp, struct dirent *restrict entry, struct dirent **restrict result) {
	*result = NULL;
	errno = 0;
	struct dirent *next = readdir(dirp);
	if (errno != 0 && next == NULL) {
		return errno;
	}
	if (next) {
		memcpy(entry, next, sizeof(struct dirent));
		*result = entry;
	}
	return 0;
}