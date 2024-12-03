#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h>

#define SCE_USER_MODULE_LIST(...)           \
char *const _sceUserModuleList[] = {        \
__VA_ARGS__                                 \
};                                          \
const int _sceUserModuleListSize = sizeof(_sceUserModuleList) / sizeof(char *);

extern const char sceUserMainThreadName[] = "pthread_test";
extern const int sceUserMainThreadPriority = SCE_KERNEL_PROCESS_PRIORITY_USER_DEFAULT;
extern const unsigned int sceUserMainThreadStackSize = 0x00040000U;

SCE_USER_MODULE_LIST("app0:sce_module/pthread.suprx");

unsigned int sceLibcHeapSize = 32 * 1024 * 1024;

#include <pthread.h>

// Function to be executed by each thread
void* print_hello(void* arg) {
    printf("Hello from thread!\n");
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Create two threads
    if (pthread_create(&thread1, NULL, print_hello, NULL) != 0) {
        perror("Failed to create thread 1");
        return 1;
    }

    if (pthread_create(&thread2, NULL, print_hello, NULL) != 0) {
        perror("Failed to create thread 2");
        return 1;
    }

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main thread finished\n");

    return 0;
}