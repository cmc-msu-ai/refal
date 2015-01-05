#pragma once

#include "Common.h"

namespace Refal2 {

class CLinkBuilder {
public:
	CLinkBuilder(): begin(0) { Reset(); }
	~CLinkBuilder() { Reset(); }

	static void Free(CUnitLink** link);

	void Reset();
	void Get(CUnitLink** link_begin, CUnitLink** link_end);
	
	CUnitLink* Char(TChar c);
	CUnitLink* Label(TLabel label);
	CUnitLink* Number(TNumber number);
	CUnitLink* Variable(TVariableIndex variable);
	CUnitLink* LeftParen();
	CUnitLink* RightParen();
	CUnitLink* LeftBracket();
	CUnitLink* RightBracket();
private:
	CLinkBuilder(const CLinkBuilder&);
	CLinkBuilder& operator=(const CLinkBuilder&);

	void link(CUnitLink* link);

	CUnitLink* begin;
	CUnitLink* end;
};

} // end of namespace Refal2

