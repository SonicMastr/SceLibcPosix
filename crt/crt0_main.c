#include <stdio.h>
#include <string.h>

#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>

#include "crt0_common.h"

#define ARGC_MAX 31

#define DEFAULT_LIBC_POSIX_PATH "app0:sce_module/SceLibcPosix.suprx"

unsigned int __dso_handle;
extern weak unsigned int _sceLdTlsDescRegionInfo;
void *_tls_region_info = &_sceLdTlsDescRegionInfo;
extern weak char *const _sceUserModuleList[];
extern weak const int _sceUserModuleListSize;
unsigned int __crt0_main_sdk_version_var = 0x03570011;

extern weak void (*__preinit_array_start[])(void);
extern weak void (*__preinit_array_end[])(void);
extern weak void (*__init_array_start[])(void);
extern weak void (*__init_array_end[])(void);

extern weak unsigned int __aeabi_unwind_cpp_pr0(void);
extern weak unsigned int __aeabi_unwind_cpp_pr1(void);
extern weak unsigned int __aeabi_unwind_cpp_pr2(void);

extern __attribute__((section(".text"))) int main(int argc, char const *argv[]);

void _initialize(unsigned int args, void *argp) {
	int ret;
	const char *argv[ARGC_MAX + 1] = { "" };
	int argc = 1;
	size_t loc = 0;
	size_t count = 0;
	char *ptr = argp;

	__dso_handle = &__dso_handle;
	__cxa_set_dso_handle_main(__dso_handle);
	if ((_tls_region_info != 0) &&
		(ret = _sceLdTlsRegisterModuleInfo(__dso_handle, _tls_region_info,
										   SCE_TLS_MAIN),
		 ret != 0)) {
		abort();
	}

	while (loc < args) {
		argv[argc] = &ptr[loc];
		loc += strnlen_s(&ptr[loc], args - loc) + 1;
		argc++;
		if (argc == ARGC_MAX) {
			break;
		}
	}

	argv[argc] = 0;

	if (_sceUserModuleList != NULL) {
		for (loc = 0; loc < _sceUserModuleListSize; loc++) {
			ret = sceKernelLoadStartModule(_sceUserModuleList[loc], 0, 0, 0, 0, 0);
			if (ret < 0) {
				sceClibPrintf("Preload module failed 0x%08x : %s\n", ret, _sceUserModuleList[loc]);
				abort();
			}
		}
	}

	ret = sceKernelLoadStartModule(DEFAULT_LIBC_POSIX_PATH, 0, 0, 0, 0, 0);
	if ((ret < 0) && (ret != 0x8002D013)) {
		sceClibPrintf("Preload SceLibcPosix failed 0x%08x : %s\n", ret, DEFAULT_LIBC_POSIX_PATH);
		abort();
	}

	count = __preinit_array_end - __preinit_array_start;
	for (loc = 0; loc < count; loc++)
		__preinit_array_start[loc]();

	count = __init_array_end - __init_array_start;
	for (loc = 0; loc < count; loc++)
		__init_array_start[loc]();

	exit(main(argc, argv));
}

int _stop(void) {
	return 0;
}

int atexit(void (*func)(void)) {
	return __cxa_atexit(func, 0, __dso_handle);
}

int at_quick_exit(void (*func)(void)) {
	return __at_quick_exit(func, 0, __dso_handle);
}

void _start(unsigned int args, void *argp) {
	_initialize(args, argp);
}
