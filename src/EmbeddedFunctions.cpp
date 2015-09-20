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

static bool readText( const CUnitList& list, std::string& text )
{
	text.clear();
	if( list.IsEmpty() || !list.GetFirst()->IsChar() ) {
		return false;
	}
	const CUnitNode* node = list.GetFirst();
	text += node->Char();
	do {
		node = node->Next();
		if( !node->IsChar() ) {
			return false;
		}
		text += node->Char();
	} while( node != list.GetLast() );
	return true;
}

static bool readNumber( const CUnitList& list, CArbitraryInteger& number )
{
	number.Zero();
	if( list.IsEmpty() ) {
		return true;
	}
	const CUnitNode* node = list.GetLast();
	for( ; node != list.GetFirst() && node->IsNumber(); node = node->Prev() ) {
		number.AddDigit( node->Number() );
	}
	if( node == list.GetFirst() ) {
		if( node->IsNumber() ) {
			number.AddDigit( node->Number() );
			return true;
		} else if( node->IsChar() && number.GetSize() != 0 ) {
			if( node->Char() == '+' ) {
				return true;
			} else if( node->Char() == '-' ) {
				number.SetSign( true /* isNegative */ );
				return true;
			}
		}
	}
	number.Zero();
	return false;
}

static bool readTwoNumbers( CUnitList& list,
	CArbitraryInteger& number1, CArbitraryInteger& number2 )
{
	if( list.IsEmpty() || !list.GetFirst()->IsLeftParen() ) {
		return false;
	}
	// list contain ( `number1` ) `number2`
	CUnitList tmp;
	CUnitNode* first = list.GetFirst();
	CUnitNode* last = list.GetFirst()->PairedParen();
	list.Detach( first, last );
	// tmp contain ( `number1` ) `number2`
	tmp.Assign( first, last );
	tmp.RemoveFirst();
	tmp.RemoveLast();
	// tmp contain `number1`, list contain `number2`
	return ( readNumber( tmp, number1 ) && readNumber( list, number2 ) );
}

static void appendNumber( CUnitList& list, const CArbitraryInteger& result )
{
	list.Empty();
	if( result.IsZero() ) {
		list.AppendNumber( 0 );
	} else {
		if( result.IsNegative() ) {
			list.AppendChar( '-' );
		}
		for( CArbitraryInteger::TDigitIndex i = result.GetSize(); i > 0; i-- ) {
			list.AppendNumber( result.GetDigit( i - 1 ) );
		}
	}
}

static bool embeddedAdd( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 ) ) {
		return false;
	}
	number1.Add( number2 );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number1 );
	return true;
}

static bool embeddedSub( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 ) ) {
		return false;
	}
	number1.Sub( number2 );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number1 );
	return true;
}

static bool embeddedMul( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 ) ) {
		return false;
	}
	number1.Mul( number2 );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number1 );
	return true;
}

static bool embeddedDiv( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 )
		|| number2.IsZero() )
	{
		return false;
	}
	number1.Div( number2 );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number1 );
	return true;
}

static bool embeddedDr( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 )
		|| number2.IsZero() )
	{
		return false;
	}
	number1.Div( number2 );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number1 );
	CUnitNode* leftParen = executionContext.Argument.AppendParens();
	CUnitList tmp;
	appendNumber( tmp, number2 );
	executionContext.Argument.InsertAfter( leftParen, tmp );
	return true;
}

static bool embeddedP1( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number;
	if( !readNumber( executionContext.Argument, number ) ) {
		return false;
	}
	number.Add( CArbitraryInteger( 1 ) );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number );
	return true;
}

static bool embeddedM1( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number;
	if( !readNumber( executionContext.Argument, number ) ) {
		return false;
	}
	number.Sub( CArbitraryInteger( 1 ) );
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number );
	return true;
}

static bool embeddedNrel( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number1;
	CArbitraryInteger number2;
	if( !readTwoNumbers( executionContext.Argument, number1, number2 ) ) {
		return false;
	}
	CUnit sign( UT_Char );
	switch( number1.Compare( number2 ) ) {
		case CArbitraryInteger::CR_Less:
			sign.Char() = '<';
			break;
		case CArbitraryInteger::CR_Equal:
			sign.Char() = '=';
			break;
		case CArbitraryInteger::CR_Great:
			sign.Char() = '>';
			break;
		default:
			assert( false );
			break;
	}
	executionContext.Argument.InsertBefore(
		executionContext.Argument.GetFirst(), sign );
	return true;
}

static bool embeddedCvb( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string text;
	CArbitraryInteger number;
	if( !readText( executionContext.Argument, text )
		|| !number.SetValueByText( text ) )
	{
		return false;
	}
	executionContext.Argument.Empty();
	appendNumber( executionContext.Argument, number );
	return true;
}

static bool embeddedCvd( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	CArbitraryInteger number;
	if( !readNumber( executionContext.Argument, number ) ) {
		return false;
	}
	std::string text;
	number.GetTextValue( text );
	executionContext.Argument.Empty();
	executionContext.Argument.AppendText( text );
	return true;
}

static bool embeddedNumb( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	return embeddedCvb( executionContext );
}

static bool embeddedSymb( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	return embeddedCvd( executionContext );
}

//-----------------------------------------------------------------------------

static bool embeddedChartof( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	std::string labelText;
	if( !readText( executionContext.Argument, labelText ) ) {
		return false;
	}
	MakeLower( labelText );
	CRuntimeFunctions& functions = executionContext.Program->Module(
		executionContext.RuntimeModuleId ).Functions;
	int labelId = functions.AddKey( labelText );
	CRuntimeFunctionPtr& function = functions.GetData( labelId );
	if( !static_cast<bool>( function ) ) {
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
	if( !readText( executionContext.Argument, filename ) || filename.empty() ) {
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
	if( !readText( executionContext.Argument, filename ) || filename.empty() ) {
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
	CUnitList tmp;
	tmp.AppendNumber( length );
	tmp.Append( executionContext.Argument );
	tmp.Move( executionContext.Argument );
	return true;
}

static bool embeddedLengr( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	TNumber length = 0;
	const CUnitNode* node = executionContext.Argument.GetFirst();
	while( node != nullptr ) {
		length++;
		assert( !( node->IsLeftBracket() || node->IsRightBracket() ) );
		node = node->Next();
	}
	CUnitList tmp;
	tmp.AppendNumber( length );
	tmp.Append( executionContext.Argument );
	tmp.Move( executionContext.Argument );
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

static bool embeddedMulte( CExecutionContext& executionContext )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( executionContext.Argument.IsEmpty()
		|| !executionContext.Argument.GetFirst()->IsNumber() )
	{
		return false;
	}
	const TNumber count = executionContext.Argument.GetFirst()->Number();
	executionContext.Argument.RemoveFirst();
	if( !executionContext.Argument.IsEmpty() ) {
		CUnitNode* lastNode = executionContext.Argument.GetLast();
		for( TNumber i = 1; i < count; i++ ) {
			executionContext.Argument.Copy( executionContext.Argument.GetLast(),
				executionContext.Argument.GetFirst(), lastNode );
		}
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
	{ "lengr", embeddedLengr },
	{ "first", embeddedFirst },
	{ "last", embeddedLast },
	{ "multe", embeddedMulte },
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
