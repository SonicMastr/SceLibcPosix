#include "vitadescriptor.h"
#include "vitafs.h"
#include "vitamman.h"

int module_start(int argc, const void *args) {
	_init_fd();
	__init_fios2();
	__init_mman();
	// __init_threads();
	return 0;
}
int module_stop(int argc, const void *args) {
	__fini_mman();
	_free_fd();
	return 0;
}
void module_exit(void) {
	__fini_mman();
	_free_fd();
}
