#include <stdlib.h>

struct _reent *_impure_ptr = NULL;
struct _reent *const _global_impure_ptr = NULL;

void __sinit(struct _reent *s) {
}