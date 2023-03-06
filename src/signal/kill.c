#include <signal.h>
#include <errno.h>

int kill(pid_t pid, int sig) {
	if (pid != sceKernelGetProcessId()) {
		errno = EPERM;
		return -1;
	}
	switch (sig) {
	default:
		__builtin_trap();
		break;
	case SIGINT:
	case SIGTERM:
		sceKernelExitProcess(-sig);
		break;
	case SIGCHLD:
	case SIGCONT:
		return 0;
	}
}