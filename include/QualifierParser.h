#pragma once

#include <Refal2.h>
#include <string>
#include <vector>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CQualifierParser : public CErrorsHelper {
public:
	CQualifierParser( IErrorHandler* errorHandler );

	void Reset( bool named = false );
	bool IsNamed() const { return named; }
	bool AddToken( const CToken& token );
	bool IsParsed() const { return parsed; }
	void Qualifier( CQualifier& qualifier );	

private:
	bool parsed;
	bool named;
	bool afterRightParen;
	CQualifierBuilder builder;
	// auxiliary functions
	void error( const CToken& token, const std::string& message );
	void addWord( const CToken& token );
	void addLabel( const CToken& token );
	void addNumber( const CToken& token );
	void addString( const CToken& token );
	void addQualifier( const CToken& token );
	void addLineFeed( const CToken& token );
	void addLeftParen( const CToken& token );
	void addRightParen( const CToken& token);
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
