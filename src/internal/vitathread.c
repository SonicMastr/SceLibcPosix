// This provides support for __getreent() as well as implementation of our thread-related wrappers

#include <reent.h>
#include <stdio.h>
#include <string.h>

#include <vitasdk/utils.h>
#include <psp2/kernel/threadmgr.h>

#define TLS_DATA	 0
#define PTHREAD_DATA 1

#define TLS_THID_PTR(thid) sceKernelGetThreadTLSAddr(thid, 0x89)
#define TLS_PTR			   sceKernelGetTLSAddr(0x89)

#define PTHREAD_THID_PTR(thid) sceKernelGetThreadTLSAddr(thid, 0x42)
#define PTHREAD_PTR			   sceKernelGetTLSAddr(0x42)

static void *__getthread_info(int thid, int type) {
	void **on_tls = NULL;

	if (type == TLS_DATA) {
		if (thid == 0)
			on_tls = TLS_PTR;
		else
			on_tls = TLS_THID_PTR(thid);
	}

	if (type == PTHREAD_DATA) {
		if (thid == 0)
			on_tls = PTHREAD_PTR;
		else
			on_tls = PTHREAD_THID_PTR(thid);
	}

	return on_tls;
}

void *vitasdk_get_tls_data(SceUID thid) {
	return __getthread_info(thid, TLS_DATA);
}

void *vitasdk_get_pthread_data(SceUID thid) {
	return __getthread_info(thid, PTHREAD_DATA);
}

int vitasdk_delete_thread_reent(int thid) {
	int res = 0;
	return res;
}
