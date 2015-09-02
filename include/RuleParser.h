#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CRuleParser : public CQualifierParser {
protected:
	CRuleParser( IErrorHandler* errorHandler = 0 );

	void Reset();
	void BeginFunction();
	void EndFunction();
	void BeginRule();
	void AddToken();

private:
	CToken functionName;
	CQualifier qualifier;
	TVariableTypeTag variableTypeTag;
	// parsing
	typedef void ( CRuleParser::*TState )();
	TState state;

	void direction();
	void afterDirection();
	void wordAfterDirection();
	void afterLeftBracket();
	void afterVariableType();
	void variableQualifier();
	void afterVariableQualifier();
	// auxiliary functions
	void error( const std::string& message );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
