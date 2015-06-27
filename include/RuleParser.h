#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CRuleParser : public CErrorsHelper {
public:
	CRuleParser( IErrorHandler* errorHandler );

	void Reset();
	void BeginFunction( const std::string& name );
	void EndFunction();
	void BeginRule();
	bool AddToken( CToken& token );
	bool IsParsed() { return parsed; }

private:
	bool parsed;

	// auxiliary functions
	void error( const CToken& token, const std::string& message );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
