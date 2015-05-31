#include <fstream>
#include <iostream>
#include <Refal2.h>

using namespace Refal2;

namespace Refal2 {

class CStandartFunctionTable : std::map<std::string, void*> {
public:
	bool FindByName( const std::string& name,
		void*& standartFunction ) const;
};

static void strangePrint( const CUnitList& expression )
{
	for( const CUnitNode* i = expression.GetFirst(); i != 0; i = i->Next() ) {
		switch( i->GetType() ) {
			case UT_Char:
				std::cout << i->Char();
				break;
			case UT_Label:
				std::cout << "'" << LabelTable.GetLabelText( i->Label() ) << "'";
				break;
			case UT_Number:
				std::cout << "'" << i->Number() << "'";
				break;
			case UT_LeftParen:
				std::cout << "(";
				break;
			case UT_RightParen:
				std::cout << ")";
				break;
			default:
				assert( false );
				break;
		}
	}
	std::cout << std::endl;
}

bool ExtrnPrint( CUnitList& argument, std::string& errorText )
{
	strangePrint( argument );
	return true;
}

bool ExtrnPrintm( CUnitList& argument, std::string& errorText )
{
	HandyPrintFieldOfView( argument );
	return true;
}

bool ExtrnProut( CUnitList& argument, std::string& errorText )
{
	strangePrint( argument );
	argument.Empty();
	return true;
}

bool ExtrnProutm( CUnitList& argument, std::string& errorText )
{
	HandyPrintFieldOfView( argument );
	argument.Empty();
	return true;
}

}

class CErrorProcessor :
	public IVariablesBuilderListener,
	public IFunctionBuilderListener,
	public IScannerListener,
	public IParserListener
{
public:
	CErrorProcessor(): parser( 0 ) {}

	virtual void OnScannerError( TScannerErrorCode errorCode, char c );
	virtual void OnParserError( TParserErrorCode errorCode );
	virtual void OnFunctionBuilderError( TFunctionBuilderErrorCode errorCode );
	virtual void OnVariablesBuilderError(
		TVariablesBuilderErrorCode errorCode );

	void SetParser( const CParser* _parser ) { parser = _parser; }
	const CParser* GetParser() const { return parser; }

private:
	const CParser* parser;
};

void CErrorProcessor::OnScannerError( TScannerErrorCode errorCode, char c )
{
	assert( parser != 0 );
	static const char* errorText[] = {
		"SEC_UnexpectedControlSequence",
		"SEC_SymbolAfterPlus",
		"SEC_UnexpectedCharacter",
		"SEC_IllegalCharacterInLabelOrNumberBegin",
		"SEC_IllegalCharacterInLabel",
		"SEC_IllegalCharacterInNumber",
		"SEC_IllegalCharacterInQualifierNameBegin",
		"SEC_IllegalCharacterInQualifierName",
		"SEC_IllegalCharacterInStringAfterBackslash",
		"SEC_IllegalCharacterInStringInHexadecimal",
		"SEC_TryingAppendNullByteToString",
		"SEC_IllegalCharacterInStringInOctal",
		"SEC_UnclosedStringConstantAtTheEndOfFile",
		"SEC_UnclosedStringConstant",
		"SEC_UnclosedLabelOrNumberAtTheEndOfFile",
		"SEC_UnclosedLabelOrNumber",
		"SEC_UnclosedLabelAtTheEndOfFile",
		"SEC_UnclosedLabel",
		"SEC_UnclosedNumberAtTheEndOfFile",
		"SEC_UnclosedNumber",
		"SEC_UnclosedQualifierAtTheEndOfFile",
		"SEC_UnclosedQualifier",
		"SEC_UnexpectedEndOfFil"
	};
	std::cout << parser->GetLineNumber() << ":"
		<< parser->GetCharOffset() << ": " << errorText[errorCode];
	if( c != '\0' ) {
		std::cout << ": " << c;
	}
	std::cout << std::endl;
}

void CErrorProcessor::OnParserError( TParserErrorCode errorCode )
{
	assert( parser != 0 );
	static const char* errorText[] = {
		"PEC_LineShouldBeginWithIdentifierOrSpace",
		"PEC_NewLineExpected",
		"PEC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine",
		"PEC_STUB"
	};
	std::cout << parser->GetLineNumber() << ":" << parser->GetCharOffset()
		<< ": " << errorText[errorCode] << std::endl;
}

void CErrorProcessor::OnFunctionBuilderError(
	TFunctionBuilderErrorCode errorCode )
{
	assert( parser != 0 );
	static const char* errorText[] = {
		"FBEC_ThereAreNoRulesInFunction",
		"FBEC_IllegalLeftBracketInLeftPart",
		"FBEC_IllegalRightBracketInLeftPart",
		"FBEC_RightParenDoesNotMatchLeftParen",
		"FBEC_RightBracketDoesNotMatchLeftBracket",
		"FBEC_UnclosedLeftParenInLeftPart",
		"FBEC_UnclosedLeftParenInRightPart",
		"FBEC_UnclosedLeftBracketInRightPart",
		"FBEC_ThereAreMultiplePartsSeparatorInRules",
		"FBEC_ThereAreNoPartsSeparatorInRules"
	};
	std::cout << parser->GetLineNumber() << ":" << parser->GetCharOffset()
		<< ": " << errorText[errorCode] << std::endl;
}

void CErrorProcessor::OnVariablesBuilderError(
	TVariablesBuilderErrorCode errorCode )
{
	assert( parser != 0 );
	static const char* errorText[] = {
		"VBEC_InvalidVatiableName",
		"VBEC_NoSuchTypeOfVariable",
		"VBEC_TypeOfVariableDoesNotMatch",
		"VBEC_NoSuchVariableInLeftPart"
	};
	std::cout << parser->GetLineNumber() << ":" << parser->GetCharOffset()
		<< ": " << errorText[errorCode] << std::endl;
}

int main( int argc, const char* argv[] )
{
	try {
		const char* filename = "../tests/print_test.ref";

		if( argc == 2 ) {
			filename = argv[1];
		}


		CErrorProcessor errorProcessor;
		CParser parser( &errorProcessor );
		errorProcessor.SetParser( &parser );

		std::ifstream f( filename );
		if( !f.good() ) {
			std::cerr << "Can't open file\n";
			return 1;
		}

		while( true ) {
			int c = f.get();
			if( c == -1 ) {
				parser.AddEndOfFile();
				break;
			} else {
				parser.AddChar(c);
			}
		}
		
		std::cout << "\n--------------------------------------------------\n\n";
		
		if( parser.HasErrors() ) {
			std::cout << "Errors!\n\n";
			return 0;
		}
		
		COperationList program;
		for( int i = LabelTable.GetFirstLabel(); i != InvalidLabel;
			i = LabelTable.GetNextLabel( i ) )
		{
			CFunction& function = LabelTable.GetLabelFunction( i );
			if( function.IsParsed() ) {
				std::cout << "{START:" << LabelTable.GetLabelText( i ) << "}\n";
				PrintFunction( function );
				CFunctionCompiler compiler;
				compiler.Compile( &function );
				std::cout << "{END:" << LabelTable.GetLabelText( i ) << "}\n";
				COperationNode* operation = program.GetLast();
				compiler.Export( program );
				operation = operation == 0 ? program.GetFirst() : operation->Next();
				function.SetCompiled( operation );
			}
		}
		
		std::cout << "\n--------------------------------------------------\n\n";
		
		TLabel entryLabel = parser.GetEntryLabel();
		if( entryLabel == InvalidLabel ) {
			entryLabel = LabelTable.AddLabel( "go" );
		}
		assert( LabelTable.GetLabelFunction( entryLabel ).IsCompiled() );
		
		CUnitList fieldOfView;
		CUnitNode* errorCall = 0;
		TExecutionResult executionResult = COperationsExecuter::Run( entryLabel,
			fieldOfView, errorCall );
		
		std::cout << "\n--------------------------------------------------\n\n";
		
		switch( executionResult ) {
			case ER_OK:
				std::cout << "OK!\nFieldOfView: ";
				break;
			case ER_RecognitionImpossible:
				std::cout << "RecognitionImpossible!\nFieldOfView: ";
				break;
			case ER_CallEmptyFunction:
				std::cout << "CallEmptyFunction!\nFieldOfView: ";
				break;
			case ER_LostFunctionLabel:
				std::cout << "LostFunctionLabel!\nFieldOfView: ";
				break;
			case ER_WrongArgumentOfExternalFunction:
				std::cout << "WrongArgumentOfExternalFunction!\nFieldOfView: ";
				break;
			default:
				assert( false );
				break;
		}
		HandyPrintFieldOfView( fieldOfView );
	} catch( bool ) {
		return 1;
	}

	return 0;
}
