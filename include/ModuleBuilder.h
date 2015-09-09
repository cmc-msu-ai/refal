#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

struct CNamedQualifierInfo {
	CToken NameToken;
	CQualifier qualifier;

	CNamedQualifierInfo( const CToken& nameToken ): NameToken( nameToken ) {}
};

typedef std::map<std::string, CNamedQualifierInfo> CNamedQualifiers;

class CModuleBuilder : public CProgramBuilder {
protected:
	CModuleBuilder( IErrorHandler* errorHandler = nullptr );

	void Reset();

	void StartModule( const CToken& startToken );
	void StartModule( const CToken& startToken, const CToken& nameToken );
	void EndModule( const CToken& endToken );
	void EndModule();

	bool GetNamedQualifier( const CToken& nameToken, CQualifier& qualifier );
	bool SetNamedQualifier( const CToken& nameToken, CQualifier& qualifier );

	TLabel Declare( const CToken& nameToken );
	void SetOrdinary( const CToken& nameToken );
	void SetOrdinary( const CToken& nameToken, CRulePtr& firstRule );
	void SetEmpty( const CToken& nameToken );
	void SetEntry( const CToken& nameToken );
	void SetEntry( const CToken& nameToken, const CToken& externalNameToken );
	void SetExternal( const CToken& nameToken );
	void SetExternal( const CToken& nameToken,
		const CToken& externalNameToken );

private:
	bool isAnonymousModule;
	CModuleDataPtr module;
	CNamedQualifiers namedQualifiers;
	// auxiliary functions
	void reset();
	bool isModuleExist() const;
	void createModule();
	bool attemptModule();
	bool checkModuleExist( const CToken& token );
	void error( const CToken& token, const std::string& message );
	void fatalError( const CToken& token, const std::string& message );
	TLabel declare( const CToken& nameToken );
	CPreparatoryFunction& getFunction( TLabel function );
	CPreparatoryFunction& addFunction( const CToken& nameToken );
	bool checkOnlyDeclared( CPreparatoryFunction& function,
		const CToken& nameToken );
	void checkModule();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
