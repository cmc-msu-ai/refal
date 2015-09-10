#include <Refal2.h>

namespace Refal2 {

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
	executionContext.Argument.AppendText( text );
	return true;
}

static bool embeddedPrint( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.StrangePrint( std::cout,
		CProgramPrintHelper( executionContext.Program ) );
	return true;
}

static bool embeddedPrintm( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.HandyPrint( std::cout,
		CProgramPrintHelper( executionContext.Program ) );
	return true;
}

static bool embeddedProut( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.StrangePrint( std::cout,
		CProgramPrintHelper( executionContext.Program ) );
	executionContext.Argument.Empty();
	return true;
}

static bool embeddedProutm( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.HandyPrint( std::cout,
		CProgramPrintHelper( executionContext.Program ) );
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
		tmp.AppendText( numberText );
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

static bool embeddedChartof( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string labelText;
	if( !readString( executionContext.Argument.GetFirst(), labelText ) ) {
		return false;
	}
	CRuntimeFunctions& functions = executionContext.Program->Module(
		executionContext.RuntimeModuleId ).Functions;
	std::string labelLowerCaseText( labelText );
	MakeLower( labelLowerCaseText );
	int labelId = functions.FindKey( labelLowerCaseText );
	if( labelId == InvalidDictionaryIndex ) {
		labelId = functions.AddKey( labelText );
		CRuntimeFunctionPtr& function = functions.GetData( labelId );
		assert( !static_cast<bool>( function ) );
		function = executionContext.Program->EmptyFunction();
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
	CProgramPrintHelper programPrintHelper( executionContext.Program );
	programPrintHelper.Label( stringStream,	label->Label() );
	std::string labelText = stringStream.str();
	executionContext.Argument.Empty();
	executionContext.Argument.AppendText( labelText );
	return true;
}

static bool embeddedFunctab( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Argument.Empty();
	return true;
}

//-----------------------------------------------------------------------------

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
	executionContext.Argument.AppendText( text );

	std::ostringstream stringStream;
	stringStream << std::endl;
	executionContext.Argument.AppendText( stringStream.str() );
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

static bool embeddedBr( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Receptacle->Burry( executionContext.Argument ) ) {
		executionContext.Argument.Empty();
		return true;
	}
	return false;
}

static bool embeddedDg( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Receptacle->DigOut( executionContext.Argument,
		executionContext.Argument );
	return true;
}

static bool embeddedCp( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Receptacle->Copy( executionContext.Argument,
		executionContext.Argument );
	return true;
}

static bool embeddedRp( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Receptacle->Replace( executionContext.Argument ) ) {
		executionContext.Argument.Empty();
		return true;
	}
	return false;
}

static bool embeddedDgall( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	executionContext.Receptacle->DigOutAll( executionContext.Argument );
	return true;
}

//-----------------------------------------------------------------------------

static bool embeddedLengw( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TNumber length = 0;
	const CUnitNode* node = executionContext.Argument.GetFirst();
	while( node != nullptr ) {
		length++;
		assert( !( node->IsLeftBracket() || node->IsRightBracket() ) );
		assert( !node->IsRightParen() );
		if( node->IsLeftParen() ) {
			node = node->PairedParen();
		}
		node = node->Next();
	}
	executionContext.Argument.Empty();
	executionContext.Argument.AppendNumber( length );
	return true;
}

const char Asterisk = '*';

static bool embeddedFirst( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Argument.IsEmpty()
		|| !executionContext.Argument.GetFirst()->IsNumber() )
	{
		return false;
	}
	TNumber length = executionContext.Argument.GetFirst()->Number();
	executionContext.Argument.RemoveFirst();
	if( executionContext.Argument.IsEmpty() ) {
		executionContext.Argument.AppendChar( Asterisk );
		return true;
	}
	CUnitNode* node = executionContext.Argument.GetFirst();
	while( length > 0 && node != nullptr ) {
		length--;
		assert( !( node->IsLeftBracket() || node->IsRightBracket() ) );
		assert( !node->IsRightParen() );
		if( node->IsLeftParen() ) {
			node = node->PairedParen();
		}
		node = node->Next();
	}
	if( length > 0 ) {
		CUnit asterisk( UT_Char );
		asterisk.Char() = Asterisk;
		executionContext.Argument.InsertBefore(
			executionContext.Argument.GetFirst(), asterisk );
	} else {
		CUnitNode* leftParen = executionContext.Argument.InsertBefore(
			executionContext.Argument.GetFirst(), CUnit( UT_LeftParen ) ); 
		CUnitNode* rightParen = nullptr;
		if( node == nullptr ) {
			rightParen = executionContext.Argument.AppendRightParen();
		} else {
			rightParen = executionContext.Argument.InsertBefore(
				node, CUnit( UT_RightParen ) );
		}
		leftParen->PairedParen() = rightParen;
		rightParen->PairedParen() = leftParen;
	}
	return true;
}

static bool embeddedLast( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Argument.IsEmpty()
		|| !executionContext.Argument.GetFirst()->IsNumber() )
	{
		return false;
	}
	TNumber length = executionContext.Argument.GetFirst()->Number();
	executionContext.Argument.RemoveFirst();
	if( executionContext.Argument.IsEmpty() ) {
		executionContext.Argument.AppendChar( Asterisk );
		return true;
	}
	CUnitNode* node = executionContext.Argument.GetLast();
	while( length > 0 && node != nullptr ) {
		length--;
		assert( !( node->IsLeftBracket() || node->IsRightBracket() ) );
		assert( !node->IsLeftParen() );
		if( node->IsRightParen() ) {
			node = node->PairedParen();
		}
		node = node->Prev();
	}
	if( length > 0 ) {
		executionContext.Argument.AppendChar( Asterisk );
	} else {
		CUnitNode* rightParen = executionContext.Argument.AppendRightParen();
		CUnitNode* leftParen = nullptr;
		if( node == nullptr ) {
			leftParen = executionContext.Argument.InsertBefore(
				executionContext.Argument.GetFirst(), CUnit( UT_LeftParen ) );
		} else {
			leftParen = executionContext.Argument.InsertAfter(
				node, CUnit( UT_LeftParen ) );
		}
		leftParen->PairedParen() = rightParen;
		rightParen->PairedParen() = leftParen;
	}
	return true;
}

//-----------------------------------------------------------------------------

static bool embeddedApply( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	notImplemented( __FUNCTION__ );
	return false;
}

//-----------------------------------------------------------------------------
// CStandartEmbeddedFunctionData

struct CStandartEmbeddedFunctionData {
	const char* const externalName;
	const TEmbeddedFunctionPtr EmbeddedFunction;
};

const CEmbeddedFunctionData embeddedFunctionDataTable[] = {
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
	// receptacle
	{ "br", embeddedBr },
	{ "dg", embeddedDg },
	{ "cp", embeddedCp },
	{ "rp", embeddedRp },
	{ "dgall", embeddedDgall },
	// lexical
	{ "lengw", embeddedLengw },
	{ "first", embeddedFirst },
	{ "last", embeddedLast },
	// apply
	{ "apply", embeddedApply },
	{ nullptr, nullptr }
};

const CEmbeddedFunctionData* EmbeddedFunctionDataTable()
{
	return embeddedFunctionDataTable;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
