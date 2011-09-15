
#ifndef _target_h_
#define _target_h_ 1

#include "platform.h"
#include <stddef.h>

// Include files
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct target_s
{
	void* context;
	void (*write)(struct target_s* target, const void* buf, size_t len);
} target_t;


DLL_VARIABLE void string_target_write(struct target_s* target, const void* buf, size_t len);


#define target_write(target, buf, len) (*((target)->write))((target), (buf), (len))

#ifdef __cplusplus
}
#endif

#endif _target_h_
