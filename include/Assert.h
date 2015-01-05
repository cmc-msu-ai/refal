#pragma once

#include <stdio.h>

#ifdef _MSC_VER
#define assert(expr) if( !(expr) ) { __debugbreak(); }
#else
#define assert(expr) if( !(expr) ) { printf("%s %d\n", __FILE__, __LINE__); throw false; }
#endif
