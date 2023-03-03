#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

uid_t geteuid() {
	return 1000;
}