#pragma once

#include "Common.h"

namespace Refal2 {

class CLinkBuilder {
public:
	CLinkBuilder(): begin(0) { Reset(); }
	~CLinkBuilder() { Reset(); }

	static void Free(CLink** link);

	void Reset();
	void Get(CLink** link_begin, CLink** link_end);
	
	CUnitLink* Char(TChar c);
	CUnitLink* Label(TLabel label);
	CUnitLink* Number(TNumber number);
	CVariableLink* Variable(TVariableType type = '\0',
		TVariableName name = '\0', CQualifier* qualifier = 0);
	CUnitLink* LeftParen();
	CUnitLink* RightParen();
	CLink* LeftBracket();
	CLink* RightBracket();
private:
	CLinkBuilder(const CLinkBuilder&);
	CLinkBuilder& operator=(const CLinkBuilder&);

	void link(CLink* link);

	CLink* begin;
	CLink* end;
};

} // end of namespace Refal2

