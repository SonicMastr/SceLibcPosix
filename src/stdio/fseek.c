#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

// May not be entirely accurate
int fseeko(FILE *f, off_t off, int whence) {
	int result;
	result = fseek(f, (long)off, whence);
	return result;
}