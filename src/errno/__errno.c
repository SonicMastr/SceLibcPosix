#include <stdlib.h>

int *__errno() {
	return _sceLibcErrnoLoc();
}