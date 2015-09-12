#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CRuleParser : public CQualifierParser {
protected:
	CRuleParser( IErrorHandler* errorHandler = nullptr );

	void Reset();
	bool BeginFunction();
	void EndFunction();
	bool BeginRule();
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
	void beginRule();
	void error( const std::string& message );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
