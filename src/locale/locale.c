#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "setlocale.h"

int __locale_mb_cur_max(void) {
	return 4;
}

char *setlocale(int category, const char *locale) {
	if (locale) {
		if (strcmp(locale, "POSIX") && strcmp(locale, "C") && strcmp(locale, ""))
			return NULL;
	}
	return "C";
}