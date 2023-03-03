#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

uid_t getuid() {
	return 1000;
}