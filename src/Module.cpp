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

private:
	const CPreparatoryFunction* preparatoryFunction;
	TEmbeddedFunctionPtr embeddedFunction;
	TRuntimeModuleId runtimeModuleId;
};

//-----------------------------------------------------------------------------
// CExternalFunctionData

class CExternalFunctionData {
public:
	CExternalFunctionData( int _globalIndex,
			CPreparatoryFunction* _preparatoryFunction ) :
		globalIndex( _globalIndex ),
		preparatoryFunction( _preparatoryFunction )
	{
		assert( globalIndex >= 0 );
		assert( preparatoryFunction != nullptr );
	}

	int GlobalIndex() const { return globalIndex; }
	CPreparatoryFunction* PreparatoryFunction() const
		{ return preparatoryFunction; }

private:
	int globalIndex;
	CPreparatoryFunction* preparatoryFunction;
};

//-----------------------------------------------------------------------------
// CPreparatoryRuntimeFunction

class CPreparatoryRuntimeFunction : public CRuntimeFunction {
public:
	CPreparatoryRuntimeFunction( CPreparatoryFunctionPtr& function,
		const TRuntimeModuleId moduleId );

	CPreparatoryFunction* PreparatoryFunction() const { return function.get(); }
	TRuntimeModuleId RuntimeModuleId() const { return moduleId; }

private:
	const CPreparatoryFunctionPtr function;
	const TRuntimeModuleId moduleId;

	static TRuntimeFunctionType convertType(
		const CPreparatoryFunction* const function );
};

CPreparatoryRuntimeFunction::CPreparatoryRuntimeFunction(
		CPreparatoryFunctionPtr& _function, const TRuntimeModuleId _moduleId ) :
CRuntimeFunction( convertType( _function.get() ) ),
	function( _function.release() ),
	moduleId( _moduleId )
{
}

TRuntimeFunctionType CPreparatoryRuntimeFunction::convertType(
	const CPreparatoryFunction* const function )
{
	assert( function != nullptr );
	switch( function->GetType() ) {
		case PFT_Ordinary:
			return RFT_Ordinary;
		case PFT_Empty:
			return RFT_Empty;
		case PFT_External:
			return RFT_External;
		case PFT_Declared:
		case PFT_Defined:
		case PFT_Compiled:
		case PFT_Embedded:
		default:
			break;
	}
	assert( false );
	return RFT_Empty;
}

//-----------------------------------------------------------------------------
// CInternalProgramBuilder

class CInternalProgramBuilder {
public:
	static CProgramPtr Build( CModuleDataVector& modules,
		CErrorsHelper& errors );

private:
	CErrorsHelper& errors;
	typedef std::vector<CExternalFunctionData> CExternals;
	CExternals externals;
	CDictionary<CGlobalFunctionData, std::string> globals;
	CProgramPtr program;

	CInternalProgramBuilder( CErrorsHelper& _errors, int numberOfModules );

	void addFunction( CPreparatoryFunction* function,
		const TRuntimeModuleId runtimeModuleId );
	void addFunctions( const CModuleData& module, CRuntimeModule& runtimeModule,
		const TRuntimeModuleId runtimeModuleId );
	void collect( CModuleDataVector& modules );
	void check();
	void compile();
	void link();
};

//-----------------------------------------------------------------------------

CInternalProgramBuilder::CInternalProgramBuilder( CErrorsHelper& _errors,
		int numberOfModules ) :
	errors( _errors ),
	program( new CProgram( numberOfModules ) )
{
	assert( static_cast<bool>( program ) );
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
}

CProgramPtr CInternalProgramBuilder::Build( CModuleDataVector& modules,
	CErrorsHelper& errors )
{
	assert( !errors.HasErrors() );
	assert( !modules.empty() );
	CInternalProgramBuilder builder( errors, modules.size() );
	builder.collect( modules );
	if( errors.HasErrors() ) {
		return nullptr;
	}
	builder.check();
	if( errors.HasErrors() ) {
		return nullptr;
	}
	builder.compile();
	assert( !errors.HasErrors() );
	builder.link();
	assert( !errors.HasErrors() );
	return CProgramPtr( builder.program.release() );
}

void CInternalProgramBuilder::addFunction( CPreparatoryFunction* function,
	const TRuntimeModuleId runtimeModuleId )
{
	const bool isExternal = function->IsExternal();
	const bool isGlobal = function->IsEntry();
	if( !( isExternal || isGlobal ) ) {
		return;
	}
	const int globalIndex = globals.AddKey( function->ExternalName() );
	if( isGlobal ) {
		assert( !isExternal );
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
	} else {
		assert( isExternal );
		externals.push_back( CExternalFunctionData( globalIndex, function ) );
	}
}
void CInternalProgramBuilder::addFunctions( const CModuleData& module,
	CRuntimeModule& runtimeModule, const TRuntimeModuleId runtimeModuleId )
{
	const CPreparatoryFunctions& functions = module.Functions;
	for( int i = 0; i < functions.Size(); i++ ) {
		CPreparatoryFunctionPtr function( functions.GetData( i ).release() );
		addFunction( function.get(), runtimeModuleId );
		CRuntimeFunctionPtr& runtimeFunction = runtimeModule.Functions.GetData(
			runtimeModule.Functions.AddKey( function->Name() ) );
		assert( !static_cast<bool>( runtimeFunction ) );
		runtimeFunction.reset( new CPreparatoryRuntimeFunction( function,
			runtimeModuleId ) );
	}
}

void CInternalProgramBuilder::collect( CModuleDataVector& modules )
{
	TRuntimeModuleId currentModuleId = 0;
	for( CModuleDataVector::const_iterator module = modules.begin();
		module != modules.end(); ++module )
	{
		addFunctions( *( *module ), program->Module( currentModuleId ),
			currentModuleId );
		currentModuleId++;
	}
	modules.clear();
}

void CInternalProgramBuilder::check()
{
	for( CExternals::const_iterator	function = externals.begin();
		function != externals.end(); ++function )
	{
		const int globalIndex = function->GlobalIndex();
		if( !globals.GetData( globalIndex ).IsDefined() ) {
			std::ostringstream stringStream;
			stringStream << "function with external name `"
				<< globals.GetKey( globalIndex )
				<< "` was not defined in program";
			errors.Error( stringStream.str() );
		}
	}
}

void CInternalProgramBuilder::compile()
{
	CFunctionCompiler compiler;
	for( int moduleId = 0; moduleId < program->NumberOfModules(); moduleId++ )
	{
		const CRuntimeFunctions& functions =
			program->Module( moduleId ).Functions;
		for( int i = 0; i < functions.Size(); i++ ) {
			const CRuntimeFunction& runtimeFunction = *functions.GetData( i );
			const CPreparatoryRuntimeFunction& function = static_cast<
				const CPreparatoryRuntimeFunction&>( runtimeFunction );
			if( function.PreparatoryFunction()->IsOrdinary() ) {
				function.PreparatoryFunction()->Compile( compiler );
			}
		}
	}
}

void CInternalProgramBuilder::link()
{
	for( CExternals::iterator function = externals.begin();
		function != externals.end(); ++function )
	{
		function->PreparatoryFunction();
		const int globalIndex = function->GlobalIndex();
		const CGlobalFunctionData& global = globals.GetData( globalIndex );
		assert( global.IsDefined() );
		if( global.IsEmbeddedFunction() ) {
			function->PreparatoryFunction()->SetEmbedded(
				global.EmbeddedFunction() );
		} else {
			assert( !global.IsPreparatoryFunction() );
			function->PreparatoryFunction()->Link(
				*global.PreparatoryFunction() );
		}
	}

	for( int moduleId = 0; moduleId < program->NumberOfModules(); moduleId++ )
	{
		const CRuntimeFunctions& functions =
			program->Module( moduleId ).Functions;
		for( int i = 0; i < functions.Size(); i++ ) {
			const CPreparatoryRuntimeFunction& function = static_cast<
				CPreparatoryRuntimeFunction&>( *functions.GetData( i ) );
			const CPreparatoryFunction* preparatoryFunction =
				function.PreparatoryFunction();
			CRuntimeFunctionPtr newRuntimeFunction;
			switch( function.Type() ) {
				case RFT_Empty:
					assert( preparatoryFunction->IsEmpty() );
					newRuntimeFunction.reset( new CEmptyFunction );
					break;
				case RFT_Embedded:
					assert( preparatoryFunction->IsEmbedded() );
					newRuntimeFunction.reset( new CEmbeddedFunction(
						preparatoryFunction->EmbeddedFunction() ) );
					break;
				case RFT_External:
					assert( preparatoryFunction->IsEmpty()
						|| preparatoryFunction->IsEmbedded()
						|| preparatoryFunction->IsCompiled() );
					// todo: ---
					//newRuntimeFunction.reset( new CExternalFunction( 0,
					//	function.RuntimeModuleId() ) );
					break;
				case RFT_Ordinary:
					assert( preparatoryFunction->IsCompiled() );
					// todo: ---
					//newRuntimeFunction.reset( new COrdinaryFunction( 0 ) );
					break;
				default:
					assert( false );
					break;
			}
			functions.GetData( i ).reset( newRuntimeFunction.release() );
		}
	}
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
