#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CQualifierParser : public CErrorsHelper {
public:
	CQualifierParser( IErrorHandler* errorHandler );

	void Reset();
	void BeginQualifer();
	void BeginNamedQualifier( const std::string& name );
	bool AddToken( CToken& token );
	bool IsParsed() const { return parsed; }
	void GetQualifier( CQualifier& qualifier );	

private:
	bool parsed;
	std::string name;
	bool afterRightParen;
	CQualifierBuilder builder;
	// named qualifiers
	typedef std::map<std::string, CQualifier> CNamedQualifiers;
	CNamedQualifiers namedQualifiers;
	// auxiliary functions
	void resetParser();
	void addNamedQualifier( const CToken& token );
	void error( const CToken& token, const std::string& message );
	void addWord( CToken& token );
	void addLabel( const CToken& token );
	void addNumber( const CToken& token );
	void addString( const CToken& token );
	void addQualifier( const CToken& token );
	void addLineFeed( const CToken& token );
	void addLeftParen( const CToken& token );
	void addRightParen( const CToken& token );
	// disallow copy construct and operator=
	CQualifierParser( const CQualifierParser& );
	CQualifierParser& operator=( const CQualifierParser& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
