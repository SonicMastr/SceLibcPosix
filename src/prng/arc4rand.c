#include <stdlib.h>
#include <stdint.h>

#include <psp2/kernel/rng.h>

unsigned int arc4random(void)
{
	unsigned int v = 0;
	sceKernelGetRandomNumber(&v, sizeof(v));
	return v;
}
