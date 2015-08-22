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
	TVariableType variableType;
	enum TState {
		S_Direction,
		S_AfterDirection,
		S_AfterLeftBracket,
		S_AfterVariableType,
		S_VariableQualifier,
		S_AfterVariableQualifier
	};
	TState state;
	// auxiliary functions
	void error( const std::string& message );
	void direction();
	void afterDirection();
	void wordAfterDirection();
	void afterLeftBracket();
	void afterVariableType();
	void variableQualifier();
	void afterVariableQualifier();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
