
#include "target.h"
#include "string_buffer.h"

// Include files
#ifdef __cplusplus
extern "C"
{
#endif



DLL_VARIABLE void string_target_write(struct target_s* target, const void* buf, size_t len)
{
	string_buffer_t* sb;
	if(0 == target)
		return;

	sb = (string_buffer_t*)target->context;
	string_buffer_appendLen(sb, (const char*)buf, len);
}


#ifdef __cplusplus
}
#endif