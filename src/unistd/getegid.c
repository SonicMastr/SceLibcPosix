#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

gid_t getegid() {
	return 1000;
}
