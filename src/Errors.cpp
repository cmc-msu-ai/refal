#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CErrorsHelper::CErrorsHelper( IErrorHandler* errorHandler )
{
	Reset();
	SetErrorProcessor( errorHandler );
}

void CErrorsHelper::Reset()
{
	hasErrors = false;
	hasWarnings = false;
}

void CErrorsHelper::SetErrorProcessor( IErrorHandler* _errorProcessor )
{
	errorHandler = _errorProcessor;
}

const IErrorHandler* CErrorsHelper::GetErrorProcessor() const
{
	return errorHandler;
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
