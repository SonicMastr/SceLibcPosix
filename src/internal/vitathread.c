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

#define MAX_THREADS 256

typedef struct thread_data {
	int thid;
	void *tls_data;
	void *pthread_data;
} thread_data;

static thread_data thread_list[MAX_THREADS];

SceKernelLwMutexWork _tls_mutex __attribute__((aligned(8)));

static inline void __clean_thread_data(void)
{
	int i;
	SceKernelThreadInfo info;

	for (i = 0; i < MAX_THREADS; ++i)
	{
		info.size = sizeof(SceKernelThreadInfo);

		if (sceKernelGetThreadInfo(thread_list[i].thid, &info) < 0)
		{
			thread_list[i].thid = 0;
		}
	}
}


static inline struct thread_data *__allocate_thread_data(void)
{
	int i;
	struct thread_data *free_thread = 0;

	for (i = 0; i < MAX_THREADS; ++i)
		if (thread_list[i].thid == 0)
		{
			free_thread = &thread_list[i];
			break;
		}

	return free_thread;
}

static int __deletethread_info(int thid)
{
	struct thread_data *free_thread = NULL;
	void **on_tls = NULL;
	
	if (thid == 0)
		on_tls = TLS_PTR;
	else
		on_tls = TLS_THID_PTR(thid);

	if (!*on_tls)
	{
		return 0;
	}

	free_thread = *on_tls;
	*on_tls = NULL;
	free_thread->thid = 0;
	free_thread->tls_data = NULL;
	free_thread->pthread_data = NULL;

	return 1;
}

static thread_data *__getthread_info(int thid) {
	struct thread_data *free_thread = NULL;
	void **on_tls = NULL;

	if (thid == 0)
		on_tls = TLS_PTR;
	else
		on_tls = TLS_THID_PTR(thid);

	if (*on_tls)
	{
		return *on_tls;
	}

	sceKernelLockLwMutex(&_tls_mutex, 1, 0);

	free_thread = __allocate_thread_data();

	if (!free_thread)
	{
		__clean_thread_data();
		free_thread = __allocate_thread_data();

		if (!free_thread)
		{
			sceClibPrintf("[VITASDK] FATAL: Exhausted all thread resources!\n");
			__builtin_trap();
		}
	}
	else {
		memset(free_thread, 0, sizeof(thread_data));
		if (thid == 0)
		{
			thid = sceKernelGetThreadId();
		}
		free_thread->thid = thid;
	}

	*on_tls = free_thread;

	sceKernelUnlockLwMutex(&_tls_mutex, 1);
	return free_thread;
}

void *vitasdk_get_tls_data(SceUID thid) {
	struct thread_data *thdata = __getthread_info(thid);
	return &thdata->tls_data;
}

void *vitasdk_get_pthread_data(SceUID thid) {
	struct thread_data *thdata = __getthread_info(thid);
	return &thdata->pthread_data;
}

int vitasdk_delete_thread_reent(int thid) {
	int res = 0;
	sceKernelLockLwMutex(&_tls_mutex, 1, 0);
	res = __deletethread_info(thid);
	sceKernelUnlockLwMutex(&_tls_mutex, 1);
	return res;
}

// Only Called from prx.c
// For Pthreads support
int __init_threads() {
	memset(thread_list, 0, sizeof(thread_list));
	sceKernelCreateLwMutex(&_tls_mutex, "TLS Access Mutex", SCE_KERNEL_MUTEX_ATTR_RECURSIVE, 1, NULL);
	thread_list[0].thid = sceKernelGetThreadId();
	sceKernelUnlockLwMutex(&_tls_mutex, 1);
	return 0;
}