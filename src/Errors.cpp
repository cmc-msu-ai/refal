#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CErrorsHelper::CErrorsHelper( IErrorHandler* errorHandler )
{
	Reset();
	SetErrorHandler( errorHandler );
}

void CErrorsHelper::Reset()
{
	hasErrors = false;
	hasWarnings = false;
}

void CErrorsHelper::Error( const std::string& errorText )
{
	assert( errorHandler != 0 );
	errorHandler->Error( errorText );
	hasErrors = true;
}
void CErrorsHelper::Warning( const std::string& warningText )
{
	assert( errorHandler != 0 );
	errorHandler->Warning( warningText );
	hasWarnings = true;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
