#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

#if 0
CModules::CModules( IErrorHandler* errorHandler ) :
	CFunctionBuilder( errorHandler )
{
	Reset();
}

void CModules::GetModules( std::queue<CModuleInfoPtr>& _modules )
{
	_modules.swap( modules );
	EmptyModules();
}

void CModules::Reset()
{
	CFunctionBuilder::Reset();
}

void CModules::EmptyModules()
{
	std::queue<CModuleInfoPtr> emptyModules;
	modules.swap( emptyModules );
}

void CModules::AddModule( CModuleInfoPtr& module )
{
	modules.push( CModuleInfoPtr( module.release() ) );
}
#endif

//-----------------------------------------------------------------------------

CModuleBuilder::CModuleBuilder( IErrorHandler* errorHandler ):
	CProgramBuilder( errorHandler )
{
	Reset();
}

void CModuleBuilder::Reset()
{
	CProgramBuilder::Reset();
	module.reset();
}

bool CModuleBuilder::IsStarted() const
{
	return static_cast<bool>( module );
}

void CModuleBuilder::StartModule()
{
	 startModule();
}

void CModuleBuilder::StartModule( const CToken& startToken )
{
	startModule();
	module->StartToken = startToken;
}

void CModuleBuilder::StartModule( const CToken& startToken,
	const CToken& nameToken )
{
	startModule();
	module->StartToken = startToken;
	module->NameToken = nameToken;
}

void CModuleBuilder::EndModule()
{
	endModule();
}

void CModuleBuilder::EndModule( const CToken& endToken )
{
	module->EndToken = endToken;
	endModule();
}

bool CModuleBuilder::GetNamedQualifier( const CToken& nameToken,
	CQualifier& qualifier )
{
	assert( nameToken.type == TT_Qualifier );
	assert( !nameToken.word.empty() );
	std::string qualifierName = nameToken.word;
	MakeLower( qualifierName );
	CNamedQualifiers::const_iterator i = namedQualifiers.find( qualifierName );
	if( i != namedQualifiers.end() ) {
		qualifier = i->second.qualifier;
		return true;
	}
	CErrorsHelper::Error( "qualifier `"	+ nameToken.word + "` wasn't defined" );
	return false;
}

bool CModuleBuilder::SetNamedQualifier( const CToken& nameToken,
	CQualifier& qualifier )
{
	assert( nameToken.type == TT_Word );
	std::string qualifierName = nameToken.word;
	MakeLower( qualifierName );
	std::pair<CNamedQualifiers::iterator, bool> insertPair =
		namedQualifiers.insert( std::make_pair( qualifierName,
			CNamedQualifierInfo( nameToken ) ) );
	if( insertPair.second ) {
		qualifier.Move( insertPair.first->second.qualifier );
		return true;
	}
	CErrorsHelper::Error( "qualifier `" + nameToken.word + "` already defined" );
	return false;
}

TLabel CModuleBuilder::Declare( const CToken& nameToken )
{
	return ( declare( nameToken ) );
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
	}
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken, CRulePtr& firstRule )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
		function.SetOrdinary( firstRule );
	}
}

void CModuleBuilder::SetEmpty( const CToken& nameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
		function.SetEmpty();
	}
}

void CModuleBuilder::SetEntry( const CToken& nameToken )
{
	SetEntry( nameToken, nameToken );
}

void CModuleBuilder::SetEntry( const CToken& nameToken,
	const CToken& externalNameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( function.IsExternal() ) {
		// TODO: error
		return;
	}
	function.SetEntry( externalNameToken );
}

void CModuleBuilder::SetExternal( const CToken& nameToken )
{
	SetExternal( nameToken, nameToken );
}

void CModuleBuilder::SetExternal( const CToken& nameToken,
	const CToken& externalNameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( !checkOnlyDeclared( function, nameToken ) ) {
		if( function.IsEntry() ) {
			function.SetDefined( nameToken );
			function.SetExternal( externalNameToken );
		} else {
			// TODO: error
		}
	}
}

TLabel CModuleBuilder::declare( const CToken& nameToken )
{
	assert( !nameToken.word.empty() );
	std::string name = nameToken.word;
	MakeLower( name );
	const TLabel label = module->Functions.AddKey( name );
	CPreparatoryFunctionPtr& function = module->Functions.GetData( label );
	if( !static_cast<bool>( function ) ) {
		function.reset( new CPreparatoryFunction( nameToken ) );
	}
	return label;
}

CPreparatoryFunction& CModuleBuilder::getFunction( TLabel function )
{
	return ( *module->Functions.GetData( function ) );
}

CPreparatoryFunction& CModuleBuilder::addFunction( const CToken& nameToken )
{
	return ( getFunction( declare( nameToken ) ) );
}

bool CModuleBuilder::checkOnlyDeclared( CPreparatoryFunction& function,
	const CToken& nameToken )
{
	if( function.IsDeclared() ) {
		return true;
	}
	// TODO: error
	return false;
}

void CModuleBuilder::checkModule()
{
	if( CErrorsHelper::HasErrors() ) {
		return;
	}
	for( TLabel label = 0; label < module->Functions.Size(); label++ ) {
		const CPreparatoryFunction& function = getFunction( label );
		assert( !function.IsDefined() );
		if( function.IsDeclared() ) {
			// TODO: error
		}
	}
}

void CModuleBuilder::startModule()
{
	assert( !IsStarted() );
	module.reset( new CModuleInfo );
}

void CModuleBuilder::endModule()
{
	assert( IsStarted() );
	checkModule();
	if( !CErrorsHelper::HasErrors() ) {
		CModuleInfoPtr savedModule( module.release() );
		Reset();
		CProgramBuilder::AddModule( savedModule );
	} else {
		Reset();
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
