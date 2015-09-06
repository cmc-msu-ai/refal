#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// Standart embedded functions table

struct CEmbeddedFunctionData {
	const char* const ExternalName;
	const TEmbeddedFunctionPtr EmbeddedFunction;
};

const CEmbeddedFunctionData* EmbeddedFunctionDataTable();

//-----------------------------------------------------------------------------

} // end of namespace Refal2

