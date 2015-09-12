#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CError

void CError::Reset()
{
	ResetSeverity();
	ResetFileName();
	ResetToken();
	ResetMessage();
}

bool CError::IsSet() const
{
	return ( Severity() != ES_None && !Message().empty() );
}

void CError::ResetSeverity()
{
	severity = ES_None;
	resetCache();
}

void CError::SetSeverity( TErrorSeverity _severity )
{
	assert( _severity != ES_None );
	severity = _severity;
	resetCache();
}

void CError::ResetFileName()
{
	fileName.clear();
	resetCache();
}

void CError::SetFileName( const std::string& _fileName )
{
	assert( !_fileName.empty() );
	fileName = _fileName;
	resetCache();
}

void CError::ResetToken()
{
	token.type = TT_None;
	resetCache();
}

void CError::SetTokenData( int line, int position,
	const std::string& wrongText )
{
	token.type = TT_Word;
	token.line = line;
	token.position = position;
	token.word = wrongText;
}

void CError::SetToken( const CToken& _token )
{
	assert( !_token.IsNone() );
	token = _token;
	resetCache();
}

void CError::ResetMessage()
{
	message.clear();
	resetCache();
}

void CError::SetMessage( const std::string& _message )
{
	assert( !_message.empty() );
	message = _message;
	resetCache();
}

const std::string& CError::UserMessage() const
{
	if( userMessage.empty() ) {
		std::ostringstream userMessageStream;
		if( !fileName.empty() ) {
			userMessageStream << fileName << ":";
		}
		if( !token.IsNone() ) {
			userMessageStream << token.line << ":" << token.position << ":";
		}
		if( !fileName.empty() || !token.IsNone() ) {
			userMessageStream << " ";
		}
		switch( Severity() ) {
			case ES_Warning:
				userMessageStream << "warning";
				break;
			case ES_Error:
				userMessageStream << "error";
				break;
			case ES_LinkError:
				userMessageStream << "link error";
				break;
			case ES_FatalError:
				userMessageStream << "fatal error";
				break;
			case ES_None:
			default:
				assert( false );
				break;
		}
		assert( !message.empty() );
		userMessageStream << ": " << message << ".";
		userMessage = userMessageStream.str();
	}
	return userMessage;
}

//-----------------------------------------------------------------------------

CErrorsHelper::CErrorsHelper( IErrorHandler* errorHandler )
{
	CError::Reset();
	Reset();
	SetErrorHandler( errorHandler );
}

void CErrorsHelper::Reset()
{
	errorHandler = nullptr;
	errorSeverity = ES_None;
}

void CErrorsHelper::SetErrorHandler( IErrorHandler* _errorHandler )
{
	errorHandler = _errorHandler;
}

bool CErrorsHelper::HasErrors() const
{
	switch( ErrorSeverity() ) {
		case ES_None:
		case ES_Warning:
			return false;
		case ES_Error:
		case ES_LinkError:
		case ES_FatalError:
			return true;
	}
	assert( false );
	return false;
}

void CErrorsHelper::RaiseError( TErrorSeverity severity,
	const std::string& message )
{
	CError::SetSeverity( severity );
	CError::SetMessage( message );
	raiseError();
	CError::ResetSeverity();
	CError::ResetMessage();
}

void CErrorsHelper::RaiseError( TErrorSeverity severity,
	const std::string& message, const CToken& token )
{
	CError::SetToken( token );
	RaiseError( severity, message );
	CError::ResetToken();
}

void CErrorsHelper::raiseError()
{
	assert( errorHandler != nullptr );
	assert( CError::IsSet() );
	const TErrorSeverity severities[] =
		{ ES_FatalError, ES_LinkError, ES_Error, ES_Warning, ES_None };
	for( int i = 0; i < sizeof( severities ) / sizeof( TErrorSeverity ); i++ ) {
		if( severities[i] == CError::Severity()
			|| severities[i] == errorSeverity )
		{
			errorSeverity = severities[i];
		}
	}
	errorHandler->Error( *this );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
