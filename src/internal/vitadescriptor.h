/*
Copyright (C) 2016, David "Davee" Morgan
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef _VITADESCRIPTOR_H
#define _VITADESCRIPTOR_H

#include <stddef.h>

#include "features.h"

#define MAX_OPEN_FILES 256

typedef enum
{
	VITA_DESCRIPTOR_FILE,
	VITA_DESCRIPTOR_DIRECTORY,
	VITA_DESCRIPTOR_SOCKET,
	VITA_DESCRIPTOR_TTY,
	VITA_DESCRIPTOR_PIPE
} DescriptorTypes;

typedef struct
{
	int sce_uid;
	DescriptorTypes type;
	int ref_count;
} DescriptorTranslation;

extern DescriptorTranslation *__fdmap[];

hidden void _init_fd(void);
hidden void _free_fd(void);

hidden int __acquire_descriptor(void);
hidden int __release_descriptor(int fd);
hidden int __duplicate_descriptor(int fd);
hidden int __descriptor_ref_count(int fd);
hidden DescriptorTranslation *__fd_grab(int fd);
hidden int __fd_drop(DescriptorTranslation *fdmap);

static inline int is_fd_valid(int fd) {
	return (fd >= 0) && (fd < MAX_OPEN_FILES) && (__fdmap[fd] != NULL);
}

#endif
