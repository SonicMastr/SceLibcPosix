#include "vitadescriptor.h"
#include "vitafs.h"

int module_start(int argc, const void *args) {
	_init_fd();
	__init_fios2();
	// __init_threads();
	return 0;
}
int module_stop(int argc, const void *args) {
	_free_fd();
	return 0;
}
void module_exit(void) {
	_free_fd();
}
