#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TErrorSeverity {
	ES_None,
	ES_Warning,
	ES_Error,
	ES_LinkError,
	ES_FatalError
};

class CError {
public:
	CError() { Reset(); }
	void Reset();
	bool IsSet() const;

	TErrorSeverity Severity() const { return severity; }
	const CToken& Token() const { return token; }
	const std::string& FileName() const { return fileName; }
	const std::string& Message() const { return message; }
	const std::string& UserMessage() const;

	void ResetSeverity();
	void SetSeverity( TErrorSeverity severity );
	void ResetFileName();
	void SetFileName( const std::string& fileName );
	void ResetToken();
	void SetTokenData( int line, int position,
		const std::string& wrongText = "" );
	void SetToken( const CToken& token );
	void ResetMessage();
	void SetMessage( const std::string& message );

private:
	TErrorSeverity severity;
	std::string fileName;
	CToken token;
	std::string message;
	mutable std::string userMessage;

	void resetCache() { userMessage.clear(); }
};

//-----------------------------------------------------------------------------

struct IErrorHandler {
	virtual void Error( const CError& error ) = 0;
};

//-----------------------------------------------------------------------------

class CErrorsHelper : public CError {
public:
	explicit CErrorsHelper( IErrorHandler* errorHandler = nullptr );
	void Reset();

	void ResetErrorHandler();
	bool HasErrorHandler() const { return ( ErrorHandler() != nullptr ); }
	void SetErrorHandler( IErrorHandler* errorHandler );
	IErrorHandler* ErrorHandler() const { return errorHandler; }

	TErrorSeverity ErrorSeverity() const { return errorSeverity; }
	bool HasErrors() const;
	void RaiseError( TErrorSeverity severity, const std::string& message );
	void RaiseError( TErrorSeverity severity, const std::string& message,
		const CToken& token );

private:
	IErrorHandler* errorHandler;
	TErrorSeverity errorSeverity;

	void raiseError();

	CErrorsHelper( const CErrorsHelper& );
	CErrorsHelper& operator=( const CErrorsHelper& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
