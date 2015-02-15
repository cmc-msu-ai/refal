#pragma once

#include <stdio.h>

#if defined( _MSC_VER ) && defined( _DEBUG )
#define assert(expr) if( !(expr) ) { __debugbreak(); }
#else
#define assert(expr) if( !(expr) ) { printf("Internal program error: %s %d\n", __FILE__, __LINE__); throw false; }
#endif
