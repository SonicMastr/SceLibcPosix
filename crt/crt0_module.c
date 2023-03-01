#include <stdio.h>
#include <string.h>

#include "crt0_common.h"

unsigned int __dso_handle;
extern weak unsigned int _sceLdTlsDescRegionInfo;
void *_tls_region_info = &_sceLdTlsDescRegionInfo;
unsigned int __crt0_module_sdk_version_var = 0x03570011;

extern weak int module_start(unsigned int, const void *);
extern weak int module_stop(unsigned int, const void *);
extern weak void module_exit(void);

extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

extern weak unsigned int __aeabi_unwind_cpp_pr0(void);
extern weak unsigned int __aeabi_unwind_cpp_pr1(void);
extern weak unsigned int __aeabi_unwind_cpp_pr2(void);

int __module_start_main(unsigned int args, void *argp) {
	int ret;
	int status = 0;
	size_t loc = 0;
	size_t count = 0;

	__dso_handle = &__dso_handle;
	if ((_tls_region_info != 0) &&
		(ret = _sceLdTlsRegisterModuleInfo(__dso_handle, _tls_region_info,
										   SCE_TLS_MODULE),
		 ret != 0)) {
		return 2;
	}

	count = __preinit_array_end - __preinit_array_start;
	for (loc = 0; loc < count; loc++)
		__preinit_array_start[loc]();

	count = __init_array_end - __init_array_start;
	for (loc = 0; loc < count; loc++)
		__init_array_start[loc]();

	if ((status = module_start(args, argp), (status == 2 || status == 1)) && (__cxa_finalize(&__dso_handle), _tls_region_info != 0) && (ret = _sceLdTlsUnregisterModuleInfo(__dso_handle), ret != 0)) {
		return 2;
	}
	return 0;
}

int __module_stop_main(unsigned int args, void *argp) {
	int ret;
	int status = 0;
	if (status = module_stop(args, argp), status != 1) {
		__cxa_finalize(&__dso_handle);
		if ((_tls_region_info != 0) && (ret = _sceLdTlsUnregisterModuleInfo(__dso_handle), ret != 0)) {
			status = 2;
		}
	}
	return status;
}

void __module_exit_main(void) {
	module_exit();

	__cxa_finalize(&__dso_handle);
	if (_tls_region_info != 0) {
		_sceLdTlsUnregisterModuleInfo(__dso_handle);
	}
}

int atexit(void (*func)(void)) {
	return __cxa_atexit(func, 0, 0);
}

int at_quick_exit(void (*func)(void)) {
	return __at_quick_exit(func, 0, 0);
}

void __module_start(unsigned int args, void *argp) {
	__module_start_main(args, argp);
}

void __module_stop(unsigned int args, void *argp) {
	__module_stop_main(args, argp);
}

void __module_exit(void) {
	__module_exit_main();
}
