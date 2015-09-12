#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CModuleBuilder::CModuleBuilder( IErrorHandler* errorHandler ):
	CProgramBuilder( errorHandler )
{
	reset();
}

void CModuleBuilder::Reset()
{
	CProgramBuilder::Reset();
	reset();
}

void CModuleBuilder::StartModule( const CToken& startToken )
{
	if( isModuleExist() ) {
		fatalError( startToken,
			( isAnonymousModule ?
			"only one anonymous module is allowed in same file" :
			"alone `start` directive, maybe you lost `end` directive before it"
			) );
		return;
	}
	isAnonymousModule = false;
	createModule();
	module->StartToken = startToken;
}

void CModuleBuilder::StartModule( const CToken& startToken,
	const CToken& nameToken )
{
	StartModule( startToken );
	if( isModuleExist() ) {
		module->NameToken = nameToken;
	}
}

void CModuleBuilder::EndModule( const CToken& endToken )
{
	if( !isAnonymousModule && isModuleExist() ) {
		module->EndToken = endToken;
		// end module, but not end of file
		EndModule();
		isAnonymousModule = false;
	} else {
		error( endToken, "alone `end` directive" );
	}
}

void CModuleBuilder::EndModule()
{
	if( isModuleExist() && !CErrorsHelper::HasErrors() ) {
		checkModule();
		CModuleDataPtr savedModule( module.release() );
		reset();
		if( !CErrorsHelper::HasErrors() ) {
			CProgramBuilder::AddModule( savedModule );
		}
	} else {
		reset();
	}
}

bool CModuleBuilder::GetNamedQualifier( const CToken& nameToken,
	CQualifier& qualifier )
{
	assert( isModuleExist() );
	assert( nameToken.type == TT_Qualifier );
	assert( !nameToken.word.empty() );
	std::string qualifierName = nameToken.word;
	MakeLower( qualifierName );
	CNamedQualifiers::const_iterator i = namedQualifiers.find( qualifierName );
	if( i != namedQualifiers.end() ) {
		qualifier = i->second.qualifier;
		return true;
	}
	error( nameToken, "qualifier `" + nameToken.word + "` wasn't defined" );
	return false;
}

bool CModuleBuilder::SetNamedQualifier( const CToken& nameToken,
	CQualifier& qualifier )
{
	if( !attemptModule() ) {
		fatalError( nameToken, "Wrong named qualifier out of module" );
		return false;
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
	error( nameToken, "qualifier `" + nameToken.word + "` already defined" );
	return false;
}

TLabel CModuleBuilder::Declare( const CToken& nameToken )
{
	if( !checkModuleExist( nameToken ) ) {
		return InvalidDictionaryIndex;
	}
	return ( declare( nameToken ) );
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken )
{
	if( !checkModuleExist( nameToken ) ) {
		return;
	}
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
	}
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken, CRulePtr& firstRule )
{
	if( !checkModuleExist( nameToken ) ) {
		return;
	}
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
		function.SetOrdinary( firstRule );
	}
}

void CModuleBuilder::SetEmpty( const CToken& nameToken )
{
	if( !checkModuleExist( nameToken ) ) {
		return;
	}
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
	if( !checkModuleExist( nameToken ) ) {
		return;
	}
	CPreparatoryFunction& function = addFunction( nameToken );
	if( function.IsExternal() ) {
		error( nameToken, "function `" + nameToken.word +
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
	if( !checkModuleExist( nameToken ) ) {
		return;
	}
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		if( !function.IsEntry() ) {
			function.SetDefined( nameToken );
			function.SetExternal( externalNameToken );
		} else {
			error( nameToken, "function `" + nameToken.word +
				"` already defined as entry" );
		}
	}
}

void CModuleBuilder::reset()
{
	isAnonymousModule = true;
	module.reset();
	namedQualifiers.clear();
}

bool CModuleBuilder::isModuleExist() const
{
	return static_cast<bool>( module );
}

void CModuleBuilder::createModule()
{
	assert( !isModuleExist() );
	module.reset( new CModuleData );
}

bool CModuleBuilder::attemptModule()
{
	if( isModuleExist() ) {
		return true;
	}
	if( isAnonymousModule ) {
		createModule();
		return true;
	}
	return false;
}

bool CModuleBuilder::checkModuleExist( const CToken& token )
{
	if( attemptModule() ) {
		return true;
	}
	fatalError( token, "Wrong function declaration out of module" );
	return false;
}

void CModuleBuilder::error( const CToken& token, const std::string& message )
{
	CErrorsHelper::RaiseError( ES_Error, message, token );
}

void CModuleBuilder::fatalError( const CToken& token,
	const std::string& message )
{
	CErrorsHelper::RaiseError( ES_FatalError, message, token );
}

TLabel CModuleBuilder::declare( const CToken& nameToken )
{
	assert( isModuleExist() );
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
		<< "` already defined in line " << function.NameToken().line;
	error( nameToken, stringStream.str() );
	return false;
}

void CModuleBuilder::checkModule()
{
	for( TLabel label = 0; label < module->Functions.Size(); label++ ) {
		const CPreparatoryFunction& function = getFunction( label );
		assert( !function.IsDefined() );
		if( function.IsDeclared() ) {
			std::ostringstream stringStream;
			stringStream << "function `" << function.NameToken().word
				<< "` was not defined in module";
			error( function.NameToken(), stringStream.str() );
		}
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
