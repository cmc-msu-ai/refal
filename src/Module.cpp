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

static bool embeddedCard( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( !executionContext.Argument.IsEmpty() ) {
		return false;
	}
	std::string text;
	std::getline( std::cin, text );
	for( std::string::const_iterator c = text.begin(); c != text.end(); ++c ) {
		executionContext.Argument.AppendChar( *c );
	}
	return true;
}

static bool embeddedPrint( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.StrangePrint( std::cout, CPrintHelper() );
	return true;
}

static bool embeddedPrintm( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.HandyPrint( std::cout, CPrintHelper() );
	return true;
}

static bool embeddedProut( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.StrangePrint( std::cout, CPrintHelper() );
	executionContext.Argument.Empty();
	return true;
}

static bool embeddedProutm( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.HandyPrint( std::cout, CPrintHelper() );
	executionContext.Argument.Empty();
	return true;
}

//-----------------------------------------------------------------------------

typedef long long int TLongSignNumber;

static bool extractNumber( const CUnitNode*& start,
	TLongSignNumber& number )
{
	assert( start != 0 );
	number = 1;
	if( start->IsChar() ) {
		switch( start->Char() ) {
			case '+':
				break;
			case '-':
				number = -1;
				break;
			default:
				return false;
		}
		start = start->Next();
	}
	if( !start->IsNumber() ) {
		return false;
	}
	number *= start->Number();
	start = start->Next();
	if( start != 0 && start->IsNumber() ) {
		notImplemented( __FUNCTION__ );
		return false;
	}
	return true;
}

static bool extractArguments( const CUnitList& argument,
	TLongSignNumber& a, TLongSignNumber& b )
{
	if( argument.IsEmpty() ) {
		return false;
	}
	const CUnitNode* start = argument.GetFirst();
	if( !start->IsLeftParen() ) {
		return false;
	}
	start = start->Next();
	if( !extractNumber( start, a ) ) {
		return false;
	}
	if( !start->IsRightParen() ) {
		return false;
	}
	start = start->Next();
	if( !extractNumber( start, b ) ) {
		return false;
	}
	if( start == 0 ) {
		return true;
	}
	return false;
}

static void setArithmeticResult( CUnitList& unitList, TLongSignNumber result )
{
	unitList.Empty();
	if( result < 0 ) {
		unitList.AppendChar( '-' );
		unitList.AppendNumber( static_cast<TNumber>( -result ) );
	} else {
		unitList.AppendNumber( static_cast<TNumber>( result ) );
	}
}

static bool embeddedAdd( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	if( extractArguments( executionContext.Argument, a, b ) ) {
		setArithmeticResult( executionContext.Argument, a + b );
		return true;
	}
	return false;
}

static bool embeddedSub( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	if( extractArguments( executionContext.Argument, a, b ) ) {
		setArithmeticResult( executionContext.Argument, a - b );
		return true;
	}
	return false;
}

static bool embeddedMul( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	if( extractArguments( executionContext.Argument, a, b ) ) {
		setArithmeticResult( executionContext.Argument, a * b );
		return true;
	}
	return false;
}

static bool embeddedDiv( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	if( extractArguments( executionContext.Argument, a, b ) ) {
		setArithmeticResult( executionContext.Argument, a / b );
		return true;
	}
	return false;
}

static bool embeddedDr( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	CUnitList& argument = executionContext.Argument;
	if( extractArguments( argument, a, b ) ) {
		argument.Empty();
		TLongSignNumber result = a / b;
		TLongSignNumber rest = a % b;
		if( result < 0 ) {
			result = -result;
			argument.AppendChar( '-' );
		}
		argument.AppendNumber( static_cast<TNumber>( result ) );
		CUnitNode* const leftParen = argument.AppendLeftParen();
		if( rest < 0 ) {
			rest = -rest;
			argument.AppendChar( '-' );
		}
		argument.AppendNumber( static_cast<TNumber>( rest ) );
		leftParen->PairedParen() = argument.AppendRightParen();
		leftParen->PairedParen()->PairedParen() = leftParen;
		return true;
	}
	return false;
}

static bool embeddedP1( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Argument.IsEmpty() ) {
		return false;
	}
	TLongSignNumber x = 0;
	const CUnitNode* start = executionContext.Argument.GetFirst();
	if( !extractNumber( start, x ) ) {
		return false;
	}
	if( start != 0 ) {
		return false;
	}
	setArithmeticResult( executionContext.Argument, x + 1 );
	return true;
}

static bool embeddedM1( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Argument.IsEmpty() ) {
		return false;
	}
	TLongSignNumber x = 0;
	const CUnitNode* start = executionContext.Argument.GetFirst();
	if( !extractNumber( start, x ) ) {
		return false;
	}
	if( start != 0 ) {
		return false;
	}
	setArithmeticResult( executionContext.Argument, x - 1 );
	return true;
}

static bool embeddedNrel( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TLongSignNumber a = 0;
	TLongSignNumber b = 0;
	if( extractArguments( executionContext.Argument, a, b ) ) {
		CUnit sign( UT_Char );
		sign.Char() = '<';
		if( a >= b ) {
			sign.Char() = ( a == b ) ? '=' : '>';
		}
		executionContext.Argument.InsertBefore(
			executionContext.Argument.GetFirst(), sign ); 
		return true;
	}
	return false;
}

static bool embeddedCvb( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string numberText;
	if( executionContext.Argument.IsEmpty() ) {
		return false;
	}
	const CUnitNode* c = executionContext.Argument.GetFirst();
	if( !c->IsChar() ) {
		return false;
	}
	bool negative = false;
	if( c->Char() == '-' || c->Char() == '+' ) {
		if( c->Char() == '-' ) {
			negative = true;
		}
		c = c->Next();
	}
	TNumber number = 0;
	for( ; c != nullptr; c = c->Next() ) {
		if( !( c->IsChar() && c->Char() >= '0' && c->Char() <= '9' ) ) {
			return false;
		}
		number = number * 10 + ( c->Char() - '0' );
	}
	executionContext.Argument.Empty();
	if( negative ) {
		executionContext.Argument.AppendChar( '-' );
	}
	executionContext.Argument.AppendNumber( number );
	return true;
}

static bool embeddedCvd( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string numberText;
	if( executionContext.Argument.IsEmpty() ) {
		return false;
	}
	const CUnitNode* i = executionContext.Argument.GetFirst();
	CUnitList tmp;
	if( i->IsChar() && ( i->Char() == '-' || i->Char() == '+' ) ) {
		if( i->Char() == '-' ) {
			tmp.AppendChar( '-' );
		}
		i = i->Next();
	}
	if( !i->IsNumber() ) {
		return false;
	} else if( i->Next() != 0 ) {
		notImplemented( __FUNCTION__ );
		return false;
	} else {
		std::ostringstream stringStream;
		stringStream << i->Number();
		std::string numberText = stringStream.str();
		for( std::string::const_iterator c = numberText.begin();
			c != numberText.end(); ++c )
		{
			tmp.AppendChar( *c );
		}
	}
	tmp.Move( executionContext.Argument );
	return true;
}

static bool embeddedNumb( CExecutionContext& executionContext )
{
	return embeddedCvb( executionContext );
}

static bool embeddedSymb( CExecutionContext& executionContext )
{
	return embeddedCvd( executionContext );
}

//-----------------------------------------------------------------------------

static bool embeddedChartof( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string labelText;
	for( CUnitNode* c = executionContext.Argument.GetFirst(); c != 0;
		c = c->Next() )
	{
		if( !c->IsChar() ) {
			return false;
		}
		labelText += c->Char();
	}
	CRuntimeFunctions& functions = executionContext.Program->Module(
		executionContext.RuntimeModuleId ).Functions;
	const int labelId = functions.FindKey( labelText );
	if( labelId == InvalidDictionaryIndex ) {
		return false;
	}
	executionContext.Argument.Empty();
	executionContext.Argument.AppendLabel( labelId
		+ LabelMask * executionContext.RuntimeModuleId );
	return true;
}

static bool embeddedFtochar( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	const CUnit* label = executionContext.Argument.GetFirst();
	if( !label->IsLabel() ) {
		return false;
	}
	std::ostringstream stringStream;
	CProgramPrintHelper( executionContext.Program ).Label( stringStream,
		label->Label() );
	std::string labelText = stringStream.str();
	executionContext.Argument.Empty();
	for( std::string::const_iterator c = labelText.begin();
		c != labelText.end(); ++c )
	{
		executionContext.Argument.AppendChar( *c );
	}
	return true;
}

static bool embeddedFunctab( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.Empty();
	return true;
}

//-----------------------------------------------------------------------------

static bool readString( const CUnitNode* start, std::string& string )
{
	string.clear();
	for( ; start != nullptr; start = start->Next() ) {
		if( !start->IsChar() ) {
			return false;
		}
		string += start->Char();
	}
	return true;
}

static std::ifstream* getStreamForReading()
{
	static std::ifstream* streamForReading = new std::ifstream;
	return streamForReading;
}

static bool embeddedOpnget( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string filename;
	if( !readString( executionContext.Argument.GetFirst(), filename )
		|| filename.empty() )
	{
		return false;
	}
	std::ifstream* streamForReading = getStreamForReading();
	if( streamForReading->is_open() ) {
		return false;
	}
	streamForReading->open( filename );
	if( !streamForReading->good() ) {
		return false;
	}
	executionContext.Argument.Empty();
	return true;
}

static bool embeddedLibget( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( !executionContext.Argument.IsEmpty() ) {
		return false;
	}
	std::ifstream* streamForReading = getStreamForReading();
	if( !streamForReading->is_open() ) {
		return false;
	}
	if( streamForReading->eof() ) {
		return true;
	}
	std::string text;
	std::getline( *streamForReading, text );
	for( std::string::const_iterator c = text.begin(); c != text.end(); ++c ) {
		executionContext.Argument.AppendChar( *c );
	}
	return true;
}

static bool embeddedClsget( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( !executionContext.Argument.IsEmpty() ) {
		return false;
	}
	std::ifstream* streamForReading = getStreamForReading();
	if( !streamForReading->is_open() ) {
		return false;
	}
	streamForReading->close();
	return true;
}

static std::ofstream* getStreamForWriting()
{
	static std::ofstream* streamForWriting = new std::ofstream;
	return streamForWriting;
}

static bool embeddedOpnput( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string filename;
	if( !readString( executionContext.Argument.GetFirst(), filename )
		|| filename.empty() )
	{
		return false;
	}
	std::ofstream* streamForWriting = getStreamForWriting();
	if( streamForWriting->is_open() ) {
		return false;
	}
	streamForWriting->open( filename );
	if( !streamForWriting->good() ) {
		return false;
	}
	executionContext.Argument.Empty();
	return true;
}

static bool embeddedLibput( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::ofstream* streamForWriting = getStreamForWriting();
	if( !streamForWriting->is_open() ) {
		return false;
	}
	const CUnitNode* c = executionContext.Argument.GetFirst();
	for( ; c != nullptr; c = c->Next() ) {
		if( c->IsChar() ) {
			*streamForWriting << c->Char();
		} else if( c->IsLeftParen() ) {
			*streamForWriting << '(';
		} else if( c->IsRightParen() ) {
			*streamForWriting << ')';
		} else {
			return false;
		}
	}
	*streamForWriting << std::endl;
	executionContext.Argument.Empty();
	return true;
}

static bool embeddedClsput( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( !executionContext.Argument.IsEmpty() ) {
		return false;
	}
	std::ofstream* streamForWriting = getStreamForWriting();
	if( !streamForWriting->is_open() ) {
		return false;
	}
	streamForWriting->close();
	return true;
}

//-----------------------------------------------------------------------------
// CStandartEmbeddedFunctionData

struct CStandartEmbeddedFunctionData {
	const char* const externalName;
	const TEmbeddedFunctionPtr EmbeddedFunction;
};

const CStandartEmbeddedFunctionData standartEmbeddedFunctions[] = {
	// input/output to standart i/o devices
	{ "card", embeddedCard },
	{ "print", embeddedPrint },
	{ "printm", embeddedPrintm },
	{ "prout", embeddedProut },
	{ "proutm", embeddedProutm },
	// arithmetic
	{ "add", embeddedAdd },
	{ "sub", embeddedSub },
	{ "mul", embeddedMul },
	{ "div", embeddedDiv },
	{ "dr", embeddedDr },
	{ "p1", embeddedP1 },
	{ "m1", embeddedM1 },
	{ "nrel", embeddedNrel },
	{ "numb", embeddedNumb },
	{ "symb", embeddedSymb },
	{ "cvb", embeddedCvb },
	{ "cvd", embeddedCvd },
	// work with labels
	{ "chartof", embeddedChartof },
	{ "ftochar", embeddedFtochar },
	{ "functab", embeddedFunctab },
	// work with files
	{ "opnget", embeddedOpnget },
	{ "libget", embeddedLibget },
	{ "clsget", embeddedClsget },
	{ "opnput", embeddedOpnput },
	{ "libput", embeddedLibput },
	{ "clsput", embeddedClsput },
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
	const int globalGoIndex = globals.AddKey( ProgramStartFunctionName );
	assert( !globals.GetData( globalGoIndex ).IsDefined() );
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
