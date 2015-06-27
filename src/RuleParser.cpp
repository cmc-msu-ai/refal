#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CRuleParser::CRuleParser( IErrorHandler* errorHandler ):
	CErrorsHelper( errorHandler )
{
	Reset();
}

void CRuleParser::Reset()
{
	parsed = false;
}

void CRuleParser::BeginFunction( const std::string& name )
{
	BeginRule();
}

void CRuleParser::EndFunction()
{
}

void CRuleParser::BeginRule()
{
	parsed = false;
}

bool CRuleParser::AddToken( CToken& token )
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
