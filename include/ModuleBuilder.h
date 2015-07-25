#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

struct CModuleInfo {
	CToken NameToken;
	CToken StartToken;
	CToken EndToken;
	CPreparatoryFunctions Functions;
};

typedef std::unique_ptr<CModuleInfo> CModuleInfoPtr;

struct CNamedQualifierInfo {
	CToken NameToken;
	CQualifier qualifier;

	CNamedQualifierInfo( const CToken& nameToken ): NameToken( nameToken ) {}
};

typedef std::map<std::string, CNamedQualifierInfo> CNamedQualifiers;

class CModuleBuilder : public CFunctionBuilder {
protected:
	CModuleBuilder( IErrorHandler* errorHandler = 0 );

	virtual void OnModuleReady( CModuleInfoPtr& module );

	void Reset();

	bool IsStarted() const;
	void StartModule();
	void StartModule( const CToken& startToken );
	void StartModule( const CToken& startToken, const CToken& nameToken );
	void EndModule();
	void EndModule( const CToken& endToken );

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
	CModuleInfoPtr module;
	CNamedQualifiers namedQualifiers;
	// auxiliary functions
	TLabel declare( const CToken& nameToken );
	CPreparatoryFunction& getFunction( TLabel function );
	CPreparatoryFunction& addFunction( const CToken& nameToken );
	bool checkOnlyDeclared( CPreparatoryFunction& function,
		const CToken& nameToken );
	void checkModule();
	void startModule();
	void endModule();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
