#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int timer_delete(timer_t timerid) {
	errno = EINVAL; // since we can't create timers, any id would be invalid
	return -1;
}