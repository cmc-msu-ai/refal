#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TErrorCriticality {
	EC_Warning,
	EC_Error,
	EC_CriticalError
};

class CError {
public:
	CError( TErrorCriticality criticality, const std::string& message );
	virtual ~CError() {}

	TErrorCriticality Criticality() const { return criticality; }
	const std::string& Message() const { return message; }

	virtual void Print( std::ostream& outputStream ) const;

private:
	TErrorCriticality criticality;
	std::string message;
};

struct CFilePosition {
	int Line;
	int Position;
};

class CPositionError : public CError {
public:
	CPositionError( TErrorCriticality criticality,
		const std::string& message, const CFilePosition& position );
};

class CFileError : public CError {
public:

	void SetFile( const std::string& file );
	const std::string& GetFile() const { return file; }
	void SetLine( int line );
	int GetLine() const { return line; }
	void SetPosition( int position );
	int GetPosition() const { return position; }

private:
	std::string file;
	int line;
	int position;
};

typedef std::unique_ptr<CError> CErrorPtr;

//-----------------------------------------------------------------------------

struct IErrorHandler {
	//virtual void Error( const CError* error ) = 0;
	virtual void Error( const std::string& errorText ) = 0;
	virtual void Warning( const std::string& warningText ) = 0;
};

//-----------------------------------------------------------------------------

class CErrorsHelper {
public:
	explicit CErrorsHelper( IErrorHandler* errorHandler = 0 );
	void Reset();

	void SetErrorHandler( IErrorHandler* errorHandler );
	IErrorHandler* GetErrorHandler() const { return errorHandler; }

	bool HasErrors() const { return hasErrors; }
	bool HasWarnings() const { return hasWarnings; }

protected:
	void Error(  const std::string& errorText );
	void Warning(  const std::string& warningText );

private:
	IErrorHandler* errorHandler;
	bool hasErrors;
	bool hasWarnings;

	CErrorsHelper( const CErrorsHelper& );
	CErrorsHelper& operator=( const CErrorsHelper& );
};

inline void CErrorsHelper::SetErrorHandler( IErrorHandler* _errorHandler )
{
	errorHandler = _errorHandler;
}

//-----------------------------------------------------------------------------

class CErrorHandlerSwitcher {
public:
	CErrorHandlerSwitcher( CErrorsHelper* errorsHelper,
		IErrorHandler* newErrorHandler );
	~CErrorHandlerSwitcher() { Revert(); }

	void Revert() const;

private:
	CErrorsHelper* errorsHelper;
	IErrorHandler* oldErrorHandler;

	CErrorHandlerSwitcher( const CErrorHandlerSwitcher& );
	CErrorHandlerSwitcher& operator=( const CErrorHandlerSwitcher& );
};

//-----------------------------------------------------------------------------

inline CErrorHandlerSwitcher::CErrorHandlerSwitcher(
		CErrorsHelper* _errorsHelper, IErrorHandler* newErrorHandler ):
	errorsHelper( _errorsHelper ),
	oldErrorHandler( errorsHelper->GetErrorHandler() )
{
	errorsHelper->SetErrorHandler( newErrorHandler );
}

inline void CErrorHandlerSwitcher::Revert() const
{
	errorsHelper->SetErrorHandler( oldErrorHandler );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
