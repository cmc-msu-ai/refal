#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CScanner : public CParser {
public:
	CScanner( IErrorHandler* errorHandler = nullptr );
	void Reset();

	void AddChar( char c );
	void AddEndOfFile();

private:
	int line;
	int position;
	// for line feed normalize
	bool afterCarriageReturn;
	// preprocessing: cut comments and pluses and extract strings
	enum TPreprocessingState {
		PS_Initial,
		PS_Plus,
		PS_PlusAfterLineFeed,
		PS_SingleLineComment,
		PS_MultilineComment,
		PS_String,
		PS_StringAfterQuote,
		PS_StringAfterBackslash,
		PS_StringOctalCodeOne,
		PS_StringOctalCodeTwo
	};
	TPreprocessingState preprocessingState;
	TPreprocessingState savedPreprocessingState;
	// for read octal code in string
	char octalCodeOne;
	char octalCodeTwo;
	// processing: extract all other tokens
	enum TState {
		S_Initial,
		S_Blank,
		S_Symbol,
		S_Label,
		S_Number,
		S_Word,
		S_BeginOfQualifier,
		S_Qualifier
	};
	TState state;
	// processing errors
	enum TError {
		E_InvalidCharacter,
		E_InvalidControlCharacter,
		E_UnexpectedCharacter,
		E_UnclosedMultilineCommentAtTheEndOfFile,
		E_UnclosedString,
		E_UnclosedLabel,
		E_UnexpectedCharacterInLabel,
		E_UnclosedNumber,
		E_UnclosedQualifier,
		E_UnexpectedCharacterInQualifier
	};
	void error( TError error, char c = '\n' );
	enum TWarning {
		W_Semicolon,
		W_SymbolAfterPlus
	};
	void warning( TWarning warning, char c );
	// auxiliary functions
	void setLineAndPositionOfToken();
	void addToken( TTokenType tokenType );
	void addTokenWithCurrentLineAndPosition( TTokenType tokenType );
	void normalizeLineFeed( char c );
	// preprocessing functions
	void preprocessing( char c );
	void preprocessingEndOfFile();
	void preprocessingInitital( char c );
	void preprocessingPlus( char c );
	void preprocessingPlusAfterLineFeed( char c );
	void preprocessingSingleLineComment( char c );
	void preprocessingMultilineComment( char c );
	void preprocessingString( char c );
	void preprocessingStringAfterQuote( char c );
	void preprocessingStringAfterBackslash( char c );
	void preprocessingStringOctalCodeOne( char c );
	void preprocessingStringOctalCodeTwo( char c );
	// processing functions
	void processing( char c );
	void processingInitial( char c );
	void processingBlank( char c );
	void processingSymbol( char c );
	void processingLabel( char c );
	void processingNumber( char c );
	void processingWord( char c );
	void processingBeginOfQualifier( char c );
	void processingQualifier( char c );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
