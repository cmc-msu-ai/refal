#include <fstream>
#include <iostream>
#include <Refal2.h>

using namespace Refal2;

class CErrorProcessor :
	public IVariablesBuilderListener,
	public IFunctionBuilderListener,
	public IScannerListener,
	public IParserListener
{
public:
	virtual void OnScannerError( const TScannerErrorCodes errorCode, char c );
	virtual void OnParserError( const TParserErrorCodes errorCode );
	virtual void OnFunctionBuilderError(
		const TFunctionBuilderErrorCodes errorCode );
	virtual void OnVariablesBuilderError(
		const TVariablesBuilderErrorCodes errorCode );
};

void CErrorProcessor::OnScannerError( const TScannerErrorCodes errorCode,
	char c )
{
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
	/*std::cout << "ScannerError: " << line << ": " << localOffset << ": ";
	if( c != '\0' ) {
		std::cout << c << ": ";
	}*/
	std::cout << errorText[errorCode] << std::endl;
}

void CErrorProcessor::OnParserError( const TParserErrorCodes errorCode )
{
	static const char* errorText[] = {
		"PEC_LineShouldBeginWithIdentifierOrSpace",
		"PEC_NewLineExpected",
		"PEC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine",
		"PEC_STUB"
	};
	std::cout << errorText[errorCode] << std::endl;
}

void CErrorProcessor::OnFunctionBuilderError(
	const Refal2::TFunctionBuilderErrorCodes errorCode )
{
	static const char* errorText[] = {
		"FBEC_ThereAreNoRulesInFunction",
		"FBEC_IllegalLeftBracketInLeftPart",
		"FBEC_IllegalRightBracketInLeftPart",
		"FBEC_RightParenDoesNotMatchLeftParen",
		"FBEC_RightBracketDoesNotMatchLeftBracket",
		"FBEC_UnclosedLeftParenInLeftPart",
		"FBEC_UnclosedLeftParenInRightPart",
		"FBEC_UnclosedLeftBracketInRightPart",
		"FBEC_IllegalQualifierInRightPart"
	};
	std::cout << errorText[errorCode] << std::endl;
}

void CErrorProcessor::OnVariablesBuilderError(
	const Refal2::TVariablesBuilderErrorCodes errorCode )
{
	static const char* errorText[] = {
		"VBEC_InvalidVatiableName",
		"VBEC_NoSuchTypeOfVariable",
		"VBEC_TypeOfVariableDoesNotMatch",
		"VBEC_NoSuchVariableInLeftPart"
	};
	std::cout << errorText[errorCode] << std::endl;
}

int main( int argc, const char* argv[] )
{
	try {
		const char* filename = "../tests/btest.ref";

		if( argc == 2 ) {
			filename = argv[1];
		}

		CErrorProcessor errorProcessor;
		CParser parser( &errorProcessor );

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
