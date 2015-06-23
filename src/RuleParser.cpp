#include <Refal2.h>
#include <sstream>

namespace Refal2 {

//-----------------------------------------------------------------------------

CRuleParser::CRuleParser( IErrorHandler* errorHandler ):
	CErrorsHelper( errorHandler )
{
}

void CRuleParser::Reset()
{
	parsed = false;
}

bool CRuleParser::AddToken( const CToken& token )
{
	assert( !parsed );
	error( token, "not implemented yet" );
	return parsed;
}

void CRuleParser::error( const CToken& token, const std::string& message )
{
	parsed = true;
	std::ostringstream errorStream;
	errorStream << ":" << token.line << ":" << token.position
		<< ": error in function rule: " << message << ".";
	CErrorsHelper::Error( errorStream.str() );
}


//-----------------------------------------------------------------------------

} // end of namespace Refal2
