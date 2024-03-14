#include <stdlib.h>

static struct _reent impure_data = _REENT_INIT (impure_data);
struct _reent *_impure_ptr = &impure_data;
struct _reent *const _global_impure_ptr = &impure_data;
