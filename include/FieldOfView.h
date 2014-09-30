#pragma once

#include "Common.h"

namespace Refal2 {

struct CFieldOfView {
	CFieldOfView();
	CFieldOfView(CUnitLink* _first, CUnitLink* _last):
		first(_first), last(_last) {}
	
	CUnitLink* First() const { return first; }
	CUnitLink* Last() const { return last; }

	void Print() const;

	static void* Alloc(int size);
	static void Free(void* what);

	static CUnitLink* Insert(CUnitLink* location, const CUnitValue& value);
	static CUnitLink* Insert(CUnitLink* location, CUnitLink* what);
	static CUnitLink* Insert(CUnitLink* location, CUnitLink* from,
		CUnitLink* to);

	static void Detach(CUnitLink* location);
	static void Detach(CUnitLink* from, CUnitLink* to);

	static void Remove(CUnitLink* location);
	static void Remove(CUnitLink* from, CUnitLink* to);
	
	static CUnitLink* Move(CUnitLink* location, CUnitLink* what);
	static CUnitLink* Move(CUnitLink* location, CUnitLink* from, CUnitLink* to);

	static CUnitLink* Copy(CUnitLink* location, CUnitLink* from, CUnitLink* to);
	static CUnitLink* Copy(CUnitLink* location, CUnitLink* what);

private:
	CUnitLink* first;
	CUnitLink* last;
};

} // end of namespace refal2
