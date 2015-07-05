#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CParser : public CRuleParser {
protected:
	CParser( IErrorHandler* errorHandler = 0 );
	void Reset();

	void AddToken();

private:
	CToken savedToken1;
	CToken savedToken2;
	// parsing
	enum TState {
		S_Initial,
		S_IgnoreLine,
		S_Word,
		S_WordBlank,
		S_WordBlankS,
		S_Blank,
		S_Directive,
		S_Qualifier,
		S_Rule
	};
	TState state;
	void parsingInitial();
	void parsingIgnoreLine();
	void parsingWord();
	void parsingWordBlank();
	void parsingWordBlankS();
	void parsingBlank();
	void parsingDirective();
	void parsingQualifier();
	void parsingRuleDirection();
	void parsingRule();

	// auxiliary functions
	void checkFinished();
	bool wordIs( const std::string& word ) const;
	// processing errors
	enum TErrorCode {
		EC_LineShouldBeginWithIdentifierOrSpace,
		EC_NewLineExpected,
		EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine,
		EC_STUB
	};
	void error( TErrorCode errorCode );

};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
