#ifndef _VITAMMAN_H
#define _VITAMMAN_H

#include <sys/types.h>

#include "features.h"

#define MMAN_VM_POOL_SIZE (16 * 1024 * 1024)

#define MMAN_PAGE_SIZE 0x1000
#define MMAN_PAGE_ALIGN(x) (((x) + MMAN_PAGE_SIZE - 1) & ~(MMAN_PAGE_SIZE - 1))

hidden void __init_mman(void);
hidden void __fini_mman(void);

hidden void *__mman_map(void *hint, size_t len, int prot, int *err);
hidden int __mman_unmap(void *addr, size_t len);
hidden int __mman_protect(void *addr, size_t len, int prot);
hidden int __mman_sync(void *addr, size_t len);
hidden int __mman_discard(void *addr, size_t len);
hidden int __mman_mapped(void *addr, size_t len);
hidden int __mman_resize(void *addr, size_t old_len, size_t new_len);
hidden int __mman_prot_of(void *addr);

#endif
