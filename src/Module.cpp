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
// CProgramPrintHelper

void CProgramPrintHelper::Label( std::ostream& outputStream,
	const TLabel label ) const
{
	const int labelModuleId = label / LabelMask;
	if( PrintLabelWithModule() ) {
		outputStream << labelModuleId << ":";
	}
	outputStream << program->Module( labelModuleId ).Functions.
		GetKey(	label % LabelMask );
}

//-----------------------------------------------------------------------------
// Standart embedded functions

static void notImplemented( const char* name )
{
	std::cout << "external function `" << name
		<< "` not implemented yet." << std::endl;
}

static bool embeddedPrint( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.StrangePrint( std::cout, CPrintHelper() );
	return true;
}

static bool embeddedPrintm( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.HandyPrint( std::cout, CPrintHelper() );
	return true;
}

static bool embeddedProut( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.StrangePrint( std::cout, CPrintHelper() );
	argument.Empty();
	return true;
}

static bool embeddedProutm( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.HandyPrint( std::cout, CPrintHelper() );
	argument.Empty();
	return true;
}

static bool embeddedChartof( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.Empty();
	return true;
}

static bool embeddedFtochar( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.Empty();
	return true;
}

static bool embeddedFunctab( CUnitList& argument )
{
	DEBUG_PRINT( __FUNCTION__ )
	argument.Empty();
	return true;
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
	{ "chartof", embeddedChartof },
	{ "ftochar", embeddedFtochar },
	{ "functab", embeddedFunctab },
	{ nullptr, nullptr }
};

//-----------------------------------------------------------------------------
// CGlobalFunctionData

class CGlobalFunctionData {
public:
	CGlobalFunctionData();

	static CRuntimeFunctionPtr EmptyFunction();

	bool IsEmbeddedFunction() const;
	bool IsPreparatoryFunction() const;
	bool IsDefined() const;

	void SetPreparatoryFunction(
		const CPreparatoryFunction* const _preparatoryFunction,
		const TRuntimeModuleId _runtimeModuleId );
	const CPreparatoryFunction* PreparatoryFunction() const;

	const TRuntimeModuleId RuntimeModuleId() const;

	void SetEmbeddedFunction( const TEmbeddedFunctionPtr _embeddedFunction );
	TEmbeddedFunctionPtr EmbeddedFunction() const;	

	CRuntimeFunctionPtr RuntimeFunction() const;

private:
	const CPreparatoryFunction* preparatoryFunction;
	TEmbeddedFunctionPtr embeddedFunction;
	TRuntimeModuleId runtimeModuleId;
	mutable CRuntimeFunctionPtr runtimeFunction;
};

//-----------------------------------------------------------------------------

CGlobalFunctionData::CGlobalFunctionData() :
	preparatoryFunction( nullptr ),
	embeddedFunction( nullptr ),
	runtimeModuleId( 0 )
{
}

CRuntimeFunctionPtr CGlobalFunctionData::EmptyFunction()
{
	static const CRuntimeFunctionPtr emptyFunction( new CEmptyFunction );
	return emptyFunction;
}

bool CGlobalFunctionData::IsEmbeddedFunction() const
{
	return ( embeddedFunction != nullptr );
}

bool CGlobalFunctionData::IsPreparatoryFunction() const
{
	return ( preparatoryFunction != nullptr );
}

bool CGlobalFunctionData::IsDefined() const
{
	return ( IsEmbeddedFunction() || IsPreparatoryFunction() );
}

void CGlobalFunctionData::SetPreparatoryFunction(
	const CPreparatoryFunction* const _preparatoryFunction,
	const TRuntimeModuleId _runtimeModuleId )
{
	assert( !IsDefined() );
	assert( _preparatoryFunction != nullptr );
	preparatoryFunction = _preparatoryFunction;
	runtimeModuleId = _runtimeModuleId;
}

const CPreparatoryFunction* CGlobalFunctionData::PreparatoryFunction() const
{
	assert( IsPreparatoryFunction() );
	return preparatoryFunction;
}

const TRuntimeModuleId CGlobalFunctionData::RuntimeModuleId() const
{
	assert( IsPreparatoryFunction() );
	return runtimeModuleId;
}

void CGlobalFunctionData::SetEmbeddedFunction(
	const TEmbeddedFunctionPtr _embeddedFunction )
{
	assert( !IsDefined() );
	assert( _embeddedFunction != nullptr );
	embeddedFunction = _embeddedFunction;
}

TEmbeddedFunctionPtr CGlobalFunctionData::EmbeddedFunction() const
{
	assert( IsEmbeddedFunction() );
	return embeddedFunction;
}

CRuntimeFunctionPtr CGlobalFunctionData::RuntimeFunction() const
{
	if( !static_cast<bool>( runtimeFunction ) ) {
		if( IsEmbeddedFunction() ) {
			runtimeFunction.reset( new CEmbeddedFunction(
				embeddedFunction ) );
		} else {
			assert( IsPreparatoryFunction() );
			if( PreparatoryFunction()->IsEmpty() ) {
				runtimeFunction = EmptyFunction();
			} else {
				runtimeFunction.reset( new COrdinaryFunction(
					PreparatoryFunction()->FirstOperation(),
					RuntimeModuleId() ) );
			}
		}
	}
	return runtimeFunction;
}

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
			return builder.program;
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
			runtimeFunction = CGlobalFunctionData::EmptyFunction();
			break;
		case PFT_Compiled:
			runtimeFunction.reset( new COrdinaryFunction(
				function->FirstOperation(), runtimeModuleId ) );
			break;
		case PFT_External:
		{
			const int globalIndex = globals.FindKey( function->ExternalName() );
			runtimeFunction = globals.GetData( globalIndex ).RuntimeFunction();
			break;
		}
		default:
			assert( false );
			break;
	}
}

void CInternalProgramBuilder::link( const CModuleDataVector& modules )
{
	const CGlobalFunctionData& goFunction = globals.GetData( globals.FindKey(
		ProgramStartFunctionName ) );
	TOperationAddress goFirstOperation =
		goFunction.PreparatoryFunction()->FirstOperation();
	TRuntimeModuleId goRuntimeModuleId = goFunction.RuntimeModuleId();
	// link all
	processModules( modules, &CInternalProgramBuilder::linkFunction );
	// set program start function
	const CRuntimeFunctions& runtimeFunctions =
		program->Module( goRuntimeModuleId ).Functions;
	TLabel goLabel = InvalidDictionaryIndex;
	for( int i = 0; i < runtimeFunctions.Size(); i++ ) {
		if( runtimeFunctions.GetData( i )->IsOrdinary()
			&& static_cast<const COrdinaryFunction*>(
				runtimeFunctions.GetData( i ).get() )->FirstOperation() ==
				goFirstOperation )
		{
			goLabel = i;
		}
	}
	assert( goLabel != InvalidDictionaryIndex );
	program->SetProgramStartFunction( goLabel + goRuntimeModuleId * LabelMask );
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

CProgramPtr CProgramBuilder::BuildProgram()
{
	return CInternalProgramBuilder::Build( modules, *this );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
