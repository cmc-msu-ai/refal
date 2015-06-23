#pragma once

#include <Refal2.h>
#include <string>
#include <vector>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CRuleParser : public CErrorsHelper {
public:
	CRuleParser( IErrorHandler* errorHandler );

	void Reset();
	bool AddToken( const CToken& token );
	bool IsParsed() { return parsed; }

private:
	bool parsed;

	// auxiliary functions
	void error( const CToken& token, const std::string& message );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
