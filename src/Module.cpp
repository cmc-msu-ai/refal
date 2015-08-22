#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CProgram::CProgram( int numberOfModules ) :
	modulesSize( numberOfModules ),
	modules( new CRuntimeModule[modulesSize] )
{
	assert( modulesSize > 0 );
	assert( modules != 0 );
}

CProgram::~CProgram()
{
	delete modules;
}

CRuntimeModule& CProgram::Module( TRuntimeModuleId moduleId )
{
	assert( moduleId >= 0 && moduleId < modulesSize );
	return modules[moduleId];
}

//-----------------------------------------------------------------------------

class CGlobalFunctionData {
public:
	CGlobalFunctionData() :
		preparatoryFunction( 0 ),
		embeddedFunction( 0 ),
		runtimeModuleId( 0 )
	{
	}

	bool IsEmbeddedFunction() const
	{
		return ( embeddedFunction != 0 );
	}

	bool IsPreparatoryFunction() const
	{
		return ( preparatoryFunction != 0 );
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
		assert( _preparatoryFunction != 0 );
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
		assert( _embeddedFunction != 0 );
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

class CExternalFunctionData {
public:
	CExternalFunctionData( int _globalIndex,
			CPreparatoryFunction* _preparatoryFunction ) :
		globalIndex( _globalIndex ),
		preparatoryFunction( _preparatoryFunction )
	{
		assert( globalIndex >= 0 );
		assert( preparatoryFunction != 0 );
	}

	int GlobalIndex() const { return globalIndex; }
	CPreparatoryFunction* PreparatoryFunction() const
		{ return preparatoryFunction; }

private:
	int globalIndex;
	CPreparatoryFunction* preparatoryFunction;
};

//-----------------------------------------------------------------------------

class CPreparatoryRuntimeFunction : public CRuntimeFunction {
public:
	CPreparatoryRuntimeFunction( CPreparatoryFunction* const function,
		const TRuntimeModuleId moduleId );

	CPreparatoryFunction* PreparatoryFunction() const { return function; }
	TRuntimeModuleId RuntimeModuleId() const { return moduleId; }

private:
	CPreparatoryFunction* const function;
	const TRuntimeModuleId moduleId;

	static TRuntimeFunctionType convertType(
		const CPreparatoryFunction* const function );
};

CPreparatoryRuntimeFunction::CPreparatoryRuntimeFunction(
		CPreparatoryFunction* const _function,
		const TRuntimeModuleId _moduleId ) :
	CRuntimeFunction( convertType( _function ) ),
	function( _function ),
	moduleId( _moduleId )
{
}

TRuntimeFunctionType CPreparatoryRuntimeFunction::convertType(
	const CPreparatoryFunction* const function )
{
	assert( function != 0 );
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

	CInternalProgramBuilder( CErrorsHelper& _errors, int numberOfModules ) :
		errors( _errors ),
		program( new CProgram( numberOfModules ) )
	{
		assert( static_cast<bool>( program ) );
	}

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
	int globalIndex = globals.AddKey( function->ExternalName() );
	if( isGlobal ) {
		assert( !isExternal );
		CGlobalFunctionData& global = globals.GetData( globalIndex );
		if( global.IsDefined() ) {
			errors.Error( "already defined" );
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
		CPreparatoryFunction* function = functions.GetData( i ).get();
		CRuntimeFunctionPtr& runtimeFunction = runtimeModule.Functions.GetData(
			runtimeModule.Functions.AddKey( function->Name() ) );
		assert( !static_cast<bool>( runtimeFunction ) );
		runtimeFunction.reset( new CPreparatoryRuntimeFunction( function,
			runtimeModuleId ) );
		addFunction( function, runtimeModuleId );
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
			errors.Error( "link error" );
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
			const CPreparatoryRuntimeFunction& function = static_cast<
				CPreparatoryRuntimeFunction&>( *functions.GetData( i ) );
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
						|| preparatoryFunction->IsOrdinary() );
					// todo: ---
					newRuntimeFunction.reset( new CExternalFunction( 0,
						function.RuntimeModuleId() ) );
					break;
				case RFT_Ordinary:
					assert( preparatoryFunction->IsOrdinary() );
					// todo: ---
					newRuntimeFunction.reset( new COrdinaryFunction( 0 ) );
					break;
				default:
					assert( false );
					break;
			}
		}
	}
}

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
