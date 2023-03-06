#include <signal.h>
#include <errno.h>

#include "features.h"

static void (**__global_sig_funcs)(int);

hidden int _init_signal() {
	int i;

	if (__global_sig_funcs == NULL) {
		__global_sig_funcs = (_sig_func_ptr *)malloc(sizeof(_sig_func_ptr) * NSIG);
		if (__global_sig_funcs == NULL)
			return -1;

		for (i = 0; i < NSIG; i++)
			__global_sig_funcs[i] = SIG_DFL;
	}

	return 0;
}

_sig_func_ptr signal(int sig, _sig_func_ptr func) {
	_sig_func_ptr old_func;

	if (sig < 0 || sig >= NSIG) {
		errno = EINVAL;
		return SIG_ERR;
	}

	if (__global_sig_funcs == NULL && _init_signal() != 0)
		return SIG_ERR;

	old_func = __global_sig_funcs[sig];
	__global_sig_funcs[sig] = func;

	return old_func;
}