#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <psp2/io/stat.h>
#include <psp2/rtc.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"

int utime(const char *path, const struct utimbuf *buf) {
	if (buf != NULL) {
		struct timeval times[2];

		times[0].tv_sec = buf->actime;
		times[0].tv_usec = 0;
		times[1].tv_sec = buf->modtime;
		times[1].tv_usec = 0;
		return utimes(path, times);
	}

	return utimes(path, (struct timeval *)0);
}