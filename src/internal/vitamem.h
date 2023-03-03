#ifndef _VITAMEM_H
#define _VITAMEM_H

#include <sys/types.h>
#include "features.h"

struct malloc_managed_size
{
	size_t max_system_size;
	size_t current_system_size;
	size_t max_inuse_size;
	size_t current_inuse_size;
	size_t reserved[4];
};

int malloc_stats(struct malloc_managed_size *mmsize);
int malloc_stats_fast(struct malloc_managed_size *mmsize);
size_t malloc_usable_size(void *p);

hidden unsigned int _get_heap_size(void);

#endif