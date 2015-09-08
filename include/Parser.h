#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CParser

class CParser : public CDirectiveParser {
protected:
	CParser( IErrorHandler* errorHandler = 0 );
	void Reset();

	void AddToken();

private:
	CToken savedToken1;
	CToken savedToken2;
	// parsing
	typedef void ( CParser::*TState )();
	TState state;

	void parsingInitial();
	void parsingIgnoreLine();
	void parsingWord();
	void parsingWordBlank();
	void parsingWordBlankS();
	void parsingBlank();
	void parsingQualifier();
	void parsingRuleDirection();
	void parsingRule();
	void parsingDirective();
	// auxiliary functions
	void checkFinished();
	void error( const std::string& message );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
