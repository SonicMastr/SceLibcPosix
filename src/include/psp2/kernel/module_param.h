#ifndef _PSP2_KERNEL_MODULE_PARAM_H_
#define _PSP2_KERNEL_MODULE_PARAM_H_

#include <psp2/types.h>

#if defined(__cplusplus)
#define SCE_USER_MODULE_LIST(...)           \
extern char *const _sceUserModuleList[] = {        \
__VA_ARGS__                                 \
};                                          \
extern const int _sceUserModuleListSize = sizeof(_sceUserModuleList) / sizeof(char *);
#else
#define SCE_USER_MODULE_LIST(...)           \
char *const _sceUserModuleList[] = {        \
__VA_ARGS__                                 \
};                                          \
const int _sceUserModuleListSize = sizeof(_sceUserModuleList) / sizeof(char *);
#endif

#endif // _PSP2_KERNEL_MODULE_PARAM_H_