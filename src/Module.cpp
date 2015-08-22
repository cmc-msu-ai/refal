#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CProgram

CProgram::CProgram( int numberOfModules ) :
	modulesSize( numberOfModules ),
	modules( new CRuntimeModule[modulesSize] )
{
	assert( modulesSize > 0 );
	assert( modules != nullptr );
}

CProgram::~CProgram()
{
	delete[] modules;
}

CRuntimeModule& CProgram::Module( TRuntimeModuleId moduleId )
{
	assert( moduleId >= 0 && moduleId < modulesSize );
	return modules[moduleId];
}

//-----------------------------------------------------------------------------
// Standart embedded functions

static void notImplemented( const char* name )
{
	std::cout << "external function `" << name << "` not implemented yet.";
}

static bool embeddedPrint()
{
	notImplemented( __FUNCTION__ );
	return false;
}

static bool embeddedPrintm()
{
	notImplemented( __FUNCTION__ );
	return false;
}

static bool embeddedProut()
{
	notImplemented( __FUNCTION__ );
	return false;
}

static bool embeddedProutm()
{
	notImplemented( __FUNCTION__ );
	return false;
}

//-----------------------------------------------------------------------------
// CStandartEmbeddedFunctionData

struct CStandartEmbeddedFunctionData {
	const char* const externalName;
	const TEmbeddedFunctionPtr EmbeddedFunction;
};

const CStandartEmbeddedFunctionData standartEmbeddedFunctions[] = {
	{ "print", embeddedPrint },
	{ "printm", embeddedPrintm },
	{ "prout", embeddedProut },
	{ "proutm", embeddedProutm },
	{ nullptr, nullptr }
};

//-----------------------------------------------------------------------------
// CGlobalFunctionData

class CGlobalFunctionData {
public:
	CGlobalFunctionData() :
		preparatoryFunction( nullptr ),
		embeddedFunction( nullptr ),
		runtimeModuleId( 0 )
	{
	}

	bool IsEmbeddedFunction() const
	{
		return ( embeddedFunction != nullptr );
	}

	bool IsPreparatoryFunction() const
	{
		return ( preparatoryFunction != nullptr );
	}

	bool IsDefined() const
	{
		return ( IsEmbeddedFunction() || IsPreparatoryFunction() ); 
	}

	void SetPreparatoryFunction(
		const CPreparatoryFunction* const _preparatoryFunction,
		const TRuntimeModuleId _runtimeModuleId )
	{
		assert( !IsDefined() );
		assert( _preparatoryFunction != nullptr );
		preparatoryFunction = _preparatoryFunction;
		runtimeModuleId = _runtimeModuleId;
	}

	const CPreparatoryFunction* PreparatoryFunction() const
	{
		assert( IsPreparatoryFunction() );
		return preparatoryFunction;
	}

	const TRuntimeModuleId RuntimeModuleId() const
	{
		assert( IsPreparatoryFunction() );
		return runtimeModuleId;
	}

	void SetEmbeddedFunction( const TEmbeddedFunctionPtr _embeddedFunction )
	{
		assert( !IsDefined() );
		assert( _embeddedFunction != nullptr );
		embeddedFunction = _embeddedFunction;
	}

	TEmbeddedFunctionPtr EmbeddedFunction() const
	{
		assert( IsEmbeddedFunction() );
		return embeddedFunction;
	}

	CRuntimeFunction* CreateRuntimeFunction() const
	{
		if( IsEmbeddedFunction() ) {
			return new CEmbeddedFunction( EmbeddedFunction() );
		}
		assert( IsPreparatoryFunction() );
		if( PreparatoryFunction()->IsEmpty() ) {
			return new CEmptyFunction;
		}
		return new COrdinaryFunction( PreparatoryFunction()->FirstOperation(),
			RuntimeModuleId() );
	}

private:
	const CPreparatoryFunction* preparatoryFunction;
	TEmbeddedFunctionPtr embeddedFunction;
	TRuntimeModuleId runtimeModuleId;
};

//-----------------------------------------------------------------------------
// CInternalProgramBuilder

const char* const ProgramStartFunctionName = "go";

class CInternalProgramBuilder {
public:
	static CProgramPtr Build( CModuleDataVector& modules,
		CErrorsHelper& errors );

private:
	CErrorsHelper& errors;
	CDictionary<CGlobalFunctionData, std::string> globals;
	CProgramPtr program;

	CInternalProgramBuilder( CErrorsHelper& _errors, int numberOfModules );

	typedef void ( CInternalProgramBuilder::*TProcessFunctionPtr )(
		CPreparatoryFunction* function,
		const TRuntimeModuleId runtimeModuleId );

	void processModules( const CModuleDataVector& modules,
		const TProcessFunctionPtr processFunction );

	void collectFunction( CPreparatoryFunction* function,
		 const TRuntimeModuleId runtimeModuleId );
	void collect( const CModuleDataVector& modules );

	void check();

	void compileFunction( CPreparatoryFunction* function,
		const TRuntimeModuleId runtimeModuleId );
	void compile( const CModuleDataVector& modules );

	void linkFunction( CPreparatoryFunction* function,
		const TRuntimeModuleId runtimeModuleId );
	void link( const CModuleDataVector& modules );
};

//-----------------------------------------------------------------------------

CInternalProgramBuilder::CInternalProgramBuilder( CErrorsHelper& _errors,
		int numberOfModules ) :
	errors( _errors ),
	program( new CProgram( numberOfModules ) )
{
	assert( static_cast<bool>( program ) );
	// add standart functions
	for( int i = 0; standartEmbeddedFunctions[i].EmbeddedFunction != nullptr;
		i++ )
	{
		assert( standartEmbeddedFunctions[i].externalName != nullptr );
		std::string externalName = standartEmbeddedFunctions[i].externalName;
		assert( !externalName.empty() );
		const int globalIndex = globals.AddKey( externalName );
		CGlobalFunctionData& global = globals.GetData( globalIndex );
		global.SetEmbeddedFunction(
			standartEmbeddedFunctions[i].EmbeddedFunction );
	}
	// add `ProgramStartFunctionName`
	assert( !globals.GetData( globals.AddKey(
		ProgramStartFunctionName ) ).IsDefined() );
}

CProgramPtr CInternalProgramBuilder::Build( CModuleDataVector& modules,
	CErrorsHelper& errors )
{
	assert( !errors.HasErrors() );
	assert( !modules.empty() );
	CInternalProgramBuilder builder( errors, modules.size() );
	builder.collect( modules );
	if( !errors.HasErrors() ) {
		builder.check();
		if( !errors.HasErrors() ) {
			builder.compile( modules );
			assert( !errors.HasErrors() );
			builder.link( modules );
			assert( !errors.HasErrors() );
			return CProgramPtr( builder.program.release() );
		}
	}
	modules.clear();
	return nullptr;
}

void CInternalProgramBuilder::processModules( const CModuleDataVector& modules,
	const TProcessFunctionPtr processFunction )
{
	TRuntimeModuleId currentModuleId = 0;
	for( CModuleDataVector::const_iterator module = modules.begin();
		module != modules.end(); ++module )
	{
		const CPreparatoryFunctions& functions = ( *module )->Functions;
		for( int i = 0; i < functions.Size(); i++ ) {
			( this->*processFunction )( functions.GetData( i ).get(),
				currentModuleId );
		}
		currentModuleId++;
	}
}

void CInternalProgramBuilder::collectFunction( CPreparatoryFunction* function,
	const TRuntimeModuleId runtimeModuleId )
{
	if( !function->IsExternal() && !function->IsEntry() ) {
		return;
	}

	const int globalIndex = globals.AddKey( function->ExternalName() );
	if( function->IsEntry() ) {
		assert( !function->IsExternal() );
		CGlobalFunctionData& global = globals.GetData( globalIndex );
		if( global.IsDefined() ) {
			std::ostringstream stringStream;
			stringStream << "function with external name `"
				<< function->ExternalNameToken().word
				<< "` already defined in program";
			errors.Error( stringStream.str() );
		} else {
			global.SetPreparatoryFunction( function, runtimeModuleId );
		}
	}
}

void CInternalProgramBuilder::collect( const CModuleDataVector& modules )
{
	processModules( modules, &CInternalProgramBuilder::collectFunction );
}

void CInternalProgramBuilder::check()
{
	for( int globalIndex = 0; globalIndex < globals.Size(); globalIndex++ )
	{
		if( !globals.GetData( globalIndex ).IsDefined() ) {
			std::ostringstream stringStream;
			stringStream << "function with external name `"
				<< globals.GetKey( globalIndex ) << "` was not defined";
			errors.Error( stringStream.str() );
		}
	}
}

void CInternalProgramBuilder::compileFunction( CPreparatoryFunction* function,
	const TRuntimeModuleId /*runtimeModuleId*/ )
{
	CFunctionCompiler compiler( program->OperationsHolder() );
	if( function->IsOrdinary() ) {
		function->Compile( compiler );
	}
}

void CInternalProgramBuilder::compile( const CModuleDataVector& modules )
{
	processModules( modules, &CInternalProgramBuilder::compileFunction );
}

void CInternalProgramBuilder::linkFunction( CPreparatoryFunction* function,
	const TRuntimeModuleId runtimeModuleId )
{
	CRuntimeFunctions& functions = program->Module( runtimeModuleId ).Functions;
	const int functionKey = functions.AddKey( function->Name() );
	CRuntimeFunctionPtr& runtimeFunction = functions.GetData( functionKey );
	assert( !static_cast<bool>( runtimeFunction ) );

	switch( function->GetType() ) {
		case PFT_Empty:
			functions.AddKey( function->Name() );
			runtimeFunction.reset( new CEmptyFunction );
			break;
		case PFT_Compiled:
			runtimeFunction.reset( new COrdinaryFunction(
				function->FirstOperation(), runtimeModuleId ) );
			break;
		case PFT_External:
		{
			const int globalIndex = globals.FindKey( function->ExternalName() );
			runtimeFunction.reset(
				globals.GetData( globalIndex ).CreateRuntimeFunction() );
			break;
		}
		default:
			assert( false );
			break;
	}
}

void CInternalProgramBuilder::link( const CModuleDataVector& modules )
{
	processModules( modules, &CInternalProgramBuilder::linkFunction );
}

//-----------------------------------------------------------------------------
// CProgramBuilder

CProgramBuilder::CProgramBuilder( IErrorHandler* errorHandler ) :
	CFunctionBuilder( errorHandler )
{
	Reset();
}

void CProgramBuilder::Reset()
{
	CFunctionBuilder::Reset();
}

void CProgramBuilder::AddModule( CModuleDataPtr& module )
{
	modules.push_back( CModuleDataPtr( module.release() ) );
}

void CProgramBuilder::BuildProgram()
{
	CInternalProgramBuilder::Build( modules, *this );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
