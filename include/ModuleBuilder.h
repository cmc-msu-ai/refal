#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

#if 0
class CModules : public CFunctionBuilder {
public:
	void GetModules( std::queue<CModuleInfoPtr>& modules );

protected:
	CModules( IErrorHandler* errorHandler = 0 );

	void Reset();

	void EmptyModules();
	void AddModule( CModuleInfoPtr& module );

private:
	std::queue<CModuleInfoPtr> modules;
};
#endif

//-----------------------------------------------------------------------------

struct CNamedQualifierInfo {
	CToken NameToken;
	CQualifier qualifier;

	CNamedQualifierInfo( const CToken& nameToken ): NameToken( nameToken ) {}
};

typedef std::map<std::string, CNamedQualifierInfo> CNamedQualifiers;

class CModuleBuilder : public CProgramBuilder {
protected:
	CModuleBuilder( IErrorHandler* errorHandler = 0 );

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
	CModuleDataPtr module;
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
