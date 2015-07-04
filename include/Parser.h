#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CParser :	public CErrorsHelper {
public:

protected:
	CToken token;

	CParser( IErrorHandler* errorHandler = 0 );
	void Reset();

	void AddToken();

private:
	CToken savedToken1;
	CToken savedToken2;
	CRuleParser ruleParser;
	CQualifierParser qualifierParser;
	// processing errors
	enum TErrorCode {
		EC_LineShouldBeginWithIdentifierOrSpace,
		EC_NewLineExpected,
		EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine,
		EC_STUB
	};
	void error( TErrorCode errorCode );	
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
	bool wordIs( const std::string& word ) const;
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
