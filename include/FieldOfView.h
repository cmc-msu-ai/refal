#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CFieldOfView : private CNodeList<CUnit> {
public:
	CFieldOfView();

	void AppendChar( TChar c );
	void AppendLabel( TLabel label );
	void AppendNumber( TNumber number );
	void AppendLeftParen();
	void AppendRightParen();
	void AppendLeftBracket();
	void AppendRightBracket();

private:
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
