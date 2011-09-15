
#include "object_private.h"
#include "itostr.h"


#ifdef __cplusplus
extern "C"
{
#endif

	
DLL_VARIABLE void object_to_json(object_t* obj, target_t* out)
{
	switch(obj->o_type) 
	{
	case object_type_string: {
			cstring_t str = object_to_string(obj, "", 0);
			target_write(out, "\"", 1);
			target_write(out, string_data(&str), string_length(&str));
			target_write(out, "\"", 1);
			break;
		}
	case object_type_table: {
			
			break;
		}
	case object_type_array: {
				size_t idx;
				target_write(out, " [ ", 3);
				for( idx = 0; idx < object_length(obj); ++ idx) {
					if(0 != idx)
						target_write(out, " , ", 3);

					object_to_json(object_element_at(obj, idx), out);
				}
				target_write(out, " ] ", 3);
			break;
		}
	default: {
		cstring_t str = object_to_string()
		}
	}
}

#ifdef __cplusplus
}
#endif

