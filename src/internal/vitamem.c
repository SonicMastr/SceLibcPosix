#include "vitamem.h"

unsigned int _get_heap_size() {
	struct malloc_managed_size size_info;
	malloc_stats_fast(&size_info);
	return size_info.max_system_size;
}