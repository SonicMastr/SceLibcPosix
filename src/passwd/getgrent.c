#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

void setgrent() {
	/* TODO: implement. */
}

void endgrent() {
	/* TODO: implement. */
}

struct group *getgrent()
{
	/* TODO: implement. */
	errno = ENOSYS;
	return NULL;
}

struct group *getgrgid(gid_t gid)
{
	/* TODO: implement. */
	errno = ENOSYS;
	return NULL;
}

struct group *getgrnam(const char *name)
{
	/* TODO: implement. */
	errno = ENOSYS;
	return NULL;
}
