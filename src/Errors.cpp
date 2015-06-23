#include <Errors.h>
#include <assert.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CErrorsHelper::CErrorsHelper( IErrorHandler* errorProcessor )
{
	Reset();
	SetErrorProcessor( errorProcessor );
}

void CErrorsHelper::Reset()
{
	hasErrors = false;
	hasWarnings = false;
}

void CErrorsHelper::SetErrorProcessor( IErrorHandler* _errorProcessor )
{
	errorProcessor = _errorProcessor;
}

const IErrorHandler* CErrorsHelper::GetErrorProcessor() const
{
	return errorProcessor;
}

void CErrorsHelper::Error( const std::string& errorText )
{
	assert( errorProcessor != 0 );
	errorProcessor->Error( errorText );
	hasErrors = true;
}
void CErrorsHelper::Warning( const std::string& warningText )
{
	assert( errorProcessor != 0 );
	errorProcessor->Warning( warningText );
	hasWarnings = true;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
