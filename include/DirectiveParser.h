#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CDirectiveParser : public CRuleParser {
protected:
	CDirectiveParser( IErrorHandler* errorHandler = nullptr );

	void Reset();
	bool StartParseIfStartDirective( const CToken& moduleName );
	bool StartParseIfDirective();
	void AddToken();

private:
	CToken directive;
	CToken savedToken;
	typedef void ( CDirectiveParser::*THandler )();
	THandler handler;
	// parsing
	enum TState {
		S_Simple,
		S_OneName,
		S_OneNameAfterName,
		S_TwoNames,
		S_TwoNamesAfterName,
		S_TwoNamesAfterLeftParen,
		S_TwoNamesAfterAlias,
		S_TwoNamesAfterRightParen
	};
	TState state;
	// auxiliary functions
	void wrongDirectiveFormat();
	void simple();
	void oneName();
	void oneNameAfterName();
	void twoNames();
	void twoNamesAfterName();
	void twoNamesAfterLeftParen();
	void twoNamesAfterAlias();
	void twoNamesAfterRightParen();

	void addEmpty();
	void addExternal();
	void addEntry();

	CDirectiveParser( const CDirectiveParser& );
	CDirectiveParser& operator=( const CDirectiveParser& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
