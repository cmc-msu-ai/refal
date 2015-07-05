#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

struct IErrorHandler {
	virtual void Error( const std::string& errorText ) = 0;
	virtual void Warning( const std::string& warningText ) = 0;
};

//-----------------------------------------------------------------------------

class CErrorsHelper {
public:
	explicit CErrorsHelper( IErrorHandler* errorHandler = 0 );
	void Reset();

	void SetErrorHandler( IErrorHandler* errorHandler );
	const IErrorHandler* GetErrorHandler() const { return errorHandler; }

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

} // end of namespace Refal2
