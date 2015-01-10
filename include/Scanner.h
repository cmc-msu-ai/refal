#pragma once

#include <string>
#include <Refal2.h>

namespace Refal2 {

enum TLexem {
	L_Blank,
	L_Comma,
	L_Equal,
	L_Label,
	L_Number,
	L_String,
	L_Newline,
	L_EndOfFile,
	L_Qualifier,
	L_LeftParen,
	L_RightParen,
	L_LeftBracket,
	L_RightBracket,
	L_Identificator
};

enum TScannerState {
	SS_BeginOfLine,
	SS_BeginBlank,
	SS_BeginComment,
	SS_BeginPlus,
	SS_BeginCommentAfterPlus,
	SS_NotBeginOfLine,
	SS_NotBeginBlank,
	SS_NotBeginComment,
	SS_NotBeginPlus,
	SS_NotBeginCommentAfterPlus,
	SS_BeginLabelOrNumber,
	SS_NotBeginLabel,
	SS_NotBeginNumber,
	SS_Identificator,
	SS_BeginQualifier,
	SS_NotBeginQualifier,
	SS_String,
	SS_StringAfterQuote,
	SS_StringAfterBackslash,
	SS_StringWaitHexadecimalCode,
	SS_StringHexadecimalCode,
	SS_StringWaitOctalCode,
	SS_StringOctalCode
};

enum TScannerErrorCodes {
	SEC_UnexpectedControlSequence,
	SEC_SymbolAfterPlus,
	SEC_UnexpectedCharacter,
	SEC_IllegalCharacterInLabelOrNumberBegin,
	SEC_IllegalCharacterInLabel,
	SEC_IllegalCharacterInNumber,
	SEC_IllegalCharacterInQualifierNameBegin,
	SEC_IllegalCharacterInQualifierName,
	SEC_IllegalCharacterInStringAfterBackslash,
	SEC_IllegalCharacterInStringInHexadecimal,
	SEC_TryingAppendNullByteToString,
	SEC_IllegalCharacterInStringInOctal,
	SEC_UnclosedStringConstantAtTheEndOfFile,
	SEC_UnclosedStringConstant,
	SEC_UnclosedLabelOrNumberAtTheEndOfFile,
	SEC_UnclosedLabelOrNumber,
	SEC_UnclosedLabelAtTheEndOfFile,
	SEC_UnclosedLabel,
	SEC_UnclosedNumberAtTheEndOfFile,
	SEC_UnclosedNumber,
	SEC_UnclosedQualifierAtTheEndOfFile,
	SEC_UnclosedQualifier,
	SEC_UnexpectedEndOfFile
};

class IScannerListener {
public:
	virtual void OnScannerError(const TScannerErrorCodes errorCode, char c) = 0;
};

class CScanner : public CFunctionBuilder, public CListener<IScannerListener> {
public:
	inline CScanner(IScannerListener* listener = 0);

	void Reset();

	int GetCharOffset() { return localOffset; }
	int GetLexemOffset() { return offset; }
	int GetLine() { return line; }

	void AddChar(char c);
	void AddEndOfFile() { processEndOfFile(); }

	static std::string ToLower(const std::string& data);

protected:
	virtual void ProcessLexem();

	int line;
	int offset;
	
	TLexem lexem;
	TLabel lexemLabel;
	TNumber lexemNumber;
	std::string lexemString;

	// insensitive to case
	inline bool identificatorIs(const std::string& toCompare) const;

private:
	void processChar(char c);
	void processEndOfFile();
	inline void error(const TScannerErrorCodes errorCode, const char c = '\0');

	TScannerState state;
	int localOffset;
	unsigned int stringCharCodeAcc;
};

inline CScanner::CScanner(IScannerListener* listener):
	CFunctionBuilder( dynamic_cast<IFunctionBuilderListener*>( listener ) ),
	CListener<IScannerListener>(listener)
{
	Reset();
}

inline bool CScanner::identificatorIs(const std::string& toCompare) const
{
	return ( lexem == L_Identificator && toCompare == ToLower( lexemString ) );
}

inline void CScanner::error(const TScannerErrorCodes errorCode, const char c)
{
	SetErrors();
	if( CListener<IScannerListener>::HasListener() ) {
		CListener<IScannerListener>::GetListener()->
			OnScannerError( errorCode, c );
	}
}

} // end of namespace Refal2
