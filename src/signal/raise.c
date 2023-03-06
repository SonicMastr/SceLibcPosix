#include <signal.h>
#include <stdint.h>

int raise(int sig) {
	int ret = kill(sceKernelGetProcessId(), sig);
	return ret;
}