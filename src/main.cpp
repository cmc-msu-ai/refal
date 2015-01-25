#include <fstream>
#include <iostream>
#include <Refal2.h>

using namespace Refal2;

class CMyClass :
	public IVariablesBuilderListener,
	public IFunctionBuilderListener,
	public IScannerListener,
	public IParserListener
{
public:
	virtual void OnScannerError(const TScannerErrorCodes errorCode, char c);
	virtual void OnParserError(const TParserErrorCodes errorCode);
	virtual void OnFunctionBuilderError(const TFunctionBuilderErrorCodes errorCode);
	virtual void OnVariablesBuilderError(const TVariablesBuilderErrorCodes errorCode);
};

void CMyClass::OnScannerError(const TScannerErrorCodes errorCode, char c)
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

	std::cout << "ScannerError: " /*<< line << ": " << localOffset << ": "*/;
	/*if( c != '\0' ) {
		std::cout << c << ": ";
	}*/
	std::cout << errorText[errorCode] << "\n";
}

void CMyClass::OnParserError(const TParserErrorCodes errorCode)
{
	static const char* errorText[] = {
		"PEC_LineShouldBeginWithIdentifierOrSpace",
		"PEC_NewLineExpected",
		"PEC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine"
	};

	std::cout << errorText[errorCode] << "\n";
}

void CMyClass::OnFunctionBuilderError(const Refal2::TFunctionBuilderErrorCodes errorCode)
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
	
	printf("CFunctionBuilder error: %s\n", errorText[errorCode]);
}

void CMyClass::OnVariablesBuilderError(const Refal2::TVariablesBuilderErrorCodes errorCode)
{
	static const char* errorText[] = {
		"VBEC_InvalidVatiableName",
		"VBEC_NoSuchTypeOfVariable",
		"VBEC_TypeOfVariableDoesNotMatch",
		"VBEC_NoSuchVariableInLeftPart"
	};
	
	printf("CVariablesBuilder error: %s\n", errorText[errorCode]);
}

int main(int argc, const char* argv[])
{
	try {
		//const char* filename = "../tests/simple.ref";
		const char* filename = "../tests/PROGRAM.REF";

		if( argc == 2 ) {
			filename = argv[1];
		}

		CMyClass ca;
		CParser parser( &ca );

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

		for( int i = LabelTable.GetFirstLabel(); i != InvalidLabel;
			i = LabelTable.GetNextLabel( i ) )
		{
			CFunction* function = LabelTable.GetLabelFunction( i );
			if( function->IsParsed() ) {
				std::cout << "{START:" << LabelTable.GetLabelText( i ) << "}\n";
				PrintFunction( *function );
				CFunctionCompiler compiler;
				compiler.Compile( function );
				std::cout << "{END:" << LabelTable.GetLabelText( i ) << "}\n";
			}
		}

	} catch(bool) {
		return 1;
	}

	return 0;
}
