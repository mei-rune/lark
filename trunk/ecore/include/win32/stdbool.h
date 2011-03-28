

#ifndef _ecore_stdbool_h_
#define _ecore_stdbool_h_ 1

#include "ecore_config.h"

#ifndef __cplusplus 

typedef enum { _Bool_must_promote_to_int = -1, false = 0, true = 1 } _Bool;
#define bool _Bool

#endif

#endif // _ecore_stdbool_h_
