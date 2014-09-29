#pragma once

#include "Operation.h"
#include "Common.h"

namespace Refal2 {

class COperations {
public:
	static COperations* LoadFromFile(const char* filename);

	bool SaveToFile(const char* filename);
};

} // end of namespace refal2
