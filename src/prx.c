#include "vitadescriptor.h"
#include "vitafs.h"
#include "vitamman.h"

int module_start(unsigned int args, const void *argp) {
	unsigned int flags = 0;
	if (args == 4 && argp) {
		flags = *(unsigned int *)argp;
	}

	_init_fd();
	if (!(flags & 1)) {
		__init_fios2();
	}
	__init_mman();
	// __init_threads();
	return 0;
}
int module_stop(unsigned int args, const void *argp) {
	__fini_mman();
	_free_fd();
	return 0;
}
void module_exit(void) {
	__fini_mman();
	_free_fd();
}
