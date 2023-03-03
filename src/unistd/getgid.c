#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

gid_t getgid() {
	return 1000;
}