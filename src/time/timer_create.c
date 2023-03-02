#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>

int timer_create(clockid_t clock_id,
				 struct sigevent *__restrict evp,
				 timer_t *__restrict timerid) {
	errno = ENOTSUP;
	return -1;
}
