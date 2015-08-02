#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CProgramBuilder::CProgramBuilder( IErrorHandler* errorHandler ) :
	CFunctionBuilder( errorHandler )
{
	Reset();
}

void CProgramBuilder::Reset()
{
	CFunctionBuilder::Reset();
}

void CProgramBuilder::AddModule( CModuleInfoPtr& module )
{
	modules.push_back( CModuleInfoPtr( module.release() ) );
}

void CProgramBuilder::BuildProgram()
{
	collect();
	check();
	compile();
	link();
}

void CProgramBuilder::addFunctions( const CPreparatoryFunctions& functions )
{
	for( int i = 0; i < functions.Size(); i++ ) {
		const CPreparatoryFunction* function = functions.GetData( i ).get();
		const bool external = function->IsExternal();
		const bool entry = function->IsEntry();
		if( !( entry || external ) ) {
			continue;
		}
		assert( !( entry && external ) );
		int index = entryFunctions.AddKey( function->Name() );
		if( entry ) {
			externalFunctions.push_back(
				CExternalFunctionInfo( index, function ) );
		} else if( function->IsEntry() ) {
			entryFunctions.GetData( index ).SetPreparatoryFunction( function );
		}
	}
}

void CProgramBuilder::collect()
{
	/*for( std::vector<CModuleInfoPtr>::const_iterator module = modules.begin();
		module != modules.end(); ++module )
	{
		addFunctions( module->Functions );
	}*/
}

void CProgramBuilder::check()
{
	for( std::vector<CExternalFunctionInfo>::const_iterator
		function = externalFunctions.begin();
		function != externalFunctions.end(); ++function )
	{
		const int index = function->ExternalFunctionIndex();
		if( !entryFunctions.GetData( index ).IsDefined() ) {
			// TODO: link error
		}
	}
}

void CProgramBuilder::compile()
{
}

void CProgramBuilder::link()
{
	for( std::vector<CExternalFunctionInfo>::const_iterator
		function = externalFunctions.begin();
		function != externalFunctions.end(); ++function )
	{
		const int index = function->ExternalFunctionIndex();
		if( !entryFunctions.GetData( index ).IsDefined() ) {
			// TODO: link error
		}
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
