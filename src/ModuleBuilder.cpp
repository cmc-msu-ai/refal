#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CModuleBuilder::CModuleBuilder( IErrorHandler* errorHandler ):
	CProgramBuilder( errorHandler )
{
	Reset();
}

void CModuleBuilder::Reset()
{
	if( IsStarted() ) {
		endModule();
	}
	CProgramBuilder::Reset();
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
	error( "qualifier `" + nameToken.word + "` wasn't defined" );
	return false;
}

bool CModuleBuilder::SetNamedQualifier( const CToken& nameToken,
	CQualifier& qualifier )
{
	if( !IsStarted() ) {
		startModule();
	}
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
	error( "qualifier `" + nameToken.word + "` already defined" );
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
		error( "function `" + nameToken.word +
			"` already defined as external" );
	} else {
		function.SetEntry( externalNameToken );
	}
}

void CModuleBuilder::SetExternal( const CToken& nameToken )
{
	SetExternal( nameToken, nameToken );
}

void CModuleBuilder::SetExternal( const CToken& nameToken,
	const CToken& externalNameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		if( !function.IsEntry() ) {
			function.SetDefined( nameToken );
			function.SetExternal( externalNameToken );
		} else {
			error( "function `" + nameToken.word +
				"` already defined as entry" );
		}
	}
}

void CModuleBuilder::error( const std::string& message )
{
	CError::SetSeverity( ES_Error );
	CError::SetMessage( message );
	CErrorsHelper::Error();
}

TLabel CModuleBuilder::declare( const CToken& nameToken )
{
	if( !IsStarted() ) {
		startModule();
	}
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
	std::ostringstream stringStream;
	stringStream << "function `" << nameToken.word
		<< "` already defined in line " << function.NameToken().line
		<< " as `" << function.NameToken().word << "`";
	error( stringStream.str() );
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
			std::ostringstream stringStream;
			CError::SetErrorPosition( function.NameToken().line,
				function.NameToken().position, function.NameToken().word );
			stringStream << "function `" << function.NameToken().word
				<< "` was not defined in module";
			error( stringStream.str() );
		}
	}
}

void CModuleBuilder::startModule()
{
	assert( !IsStarted() );
	module.reset( new CModuleData );
}

void CModuleBuilder::endModule()
{
	assert( IsStarted() );
	checkModule();
	CModuleDataPtr savedModule( module.release() );
	Reset();
	if( !CErrorsHelper::HasErrors() && static_cast<bool>( savedModule ) ) {
		CProgramBuilder::AddModule( savedModule );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
