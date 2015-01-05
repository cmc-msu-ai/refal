#pragma once

#include <stdio.h>

#define assert(expr) if( !(expr) ) { printf("%s %d\n", __FILE__, __LINE__); __debugbreak(); }
