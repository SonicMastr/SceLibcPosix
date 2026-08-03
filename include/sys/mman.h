#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define MAP_FAILED ((void *)-1)

#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_TYPE      0x0f
#define MAP_FIXED     0x10
#define MAP_ANON      0x20
#define MAP_ANONYMOUS MAP_ANON
#define MAP_NORESERVE 0x4000
#define MAP_FILE      0

#define PROT_NONE  0
#define PROT_READ  1
#define PROT_WRITE 2
#define PROT_EXEC  4

#define MS_ASYNC      1
#define MS_INVALIDATE 2
#define MS_SYNC       4

#define POSIX_MADV_NORMAL     0
#define POSIX_MADV_RANDOM     1
#define POSIX_MADV_SEQUENTIAL 2
#define POSIX_MADV_WILLNEED   3
#define POSIX_MADV_DONTNEED   4

#define MADV_NORMAL      0
#define MADV_RANDOM      1
#define MADV_SEQUENTIAL  2
#define MADV_WILLNEED    3
#define MADV_DONTNEED    4
#define MADV_FREE        8
#define MADV_REMOVE      9
#define MADV_DONTFORK    10
#define MADV_DOFORK      11
#define MADV_MERGEABLE   12
#define MADV_UNMERGEABLE 13
#define MADV_HUGEPAGE    14
#define MADV_NOHUGEPAGE  15
#define MADV_DONTDUMP    16
#define MADV_DODUMP      17
#define MADV_WIPEONFORK  18
#define MADV_KEEPONFORK  19

#define MCL_CURRENT 1
#define MCL_FUTURE  2
#define MCL_ONFAULT 4

#define MREMAP_MAYMOVE 1
#define MREMAP_FIXED   2

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off);
int munmap(void *addr, size_t len);
int mprotect(void *addr, size_t len, int prot);
int msync(void *addr, size_t len, int flags);
int madvise(void *addr, size_t len, int advice);
int mincore(void *addr, size_t len, unsigned char *vec);
int posix_madvise(void *addr, size_t len, int advice);
int mlock(const void *addr, size_t len);
int munlock(const void *addr, size_t len);
int mlockall(int flags);
int munlockall(void);
void *mremap(void *old_addr, size_t old_len, size_t new_len, int flags, ...);

#ifdef __cplusplus
}
#endif

#endif
