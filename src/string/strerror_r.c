#include <string.h>

// Needed for some libc++ stuff
int __xpg_strerror_r(int err, char *buf, size_t buflen)
{
	return strerror_s(buf, buflen, err);
}
