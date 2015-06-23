#pragma once

#include <string>

namespace Refal2 {

//-----------------------------------------------------------------------------

struct IErrorHandler {
	virtual void Error( const std::string& errorText ) = 0;
	virtual void Warning( const std::string& warningText ) = 0;
};

//-----------------------------------------------------------------------------

class CErrorsHelper {
public:
	explicit CErrorsHelper( IErrorHandler* errorProcessor = 0 );
	void Reset();

	void SetErrorProcessor( IErrorHandler* errorProcessor );
	const IErrorHandler* GetErrorProcessor() const;

	bool HasErrors() const { return hasErrors; }
	bool HasWarnings() const { return hasWarnings; }

protected:
	void Error(  const std::string& errorText );
	void Warning(  const std::string& warningText );

private:
	IErrorHandler* errorProcessor;
	bool hasErrors;
	bool hasWarnings;

	CErrorsHelper( const CErrorsHelper& );
	CErrorsHelper& operator=( const CErrorsHelper& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
