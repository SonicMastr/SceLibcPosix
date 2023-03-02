#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "vitafs.h"

FILE *fopen(const char *restrict filename, const char *restrict mode) {
	FILE *f;
	char *full_path = __realpath(filename);
	if (!full_path) {
		return 0;
	}

	f = sceLibcFopen(full_path, mode);

	free(full_path);

	if (f)
		return f;

	return 0;
}
