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
	SS_S, SS_SA, SS_SB, SS_SX, SS_S0, SS_SXA, SS_S0A,
	SS_BeginLabelOrNumber,
	SS_NotBeginLabel,
	SS_NotBeginNumber,
	SS_Identificator,
	SS_BeginQualifier,
	SS_NotBeginQualifier
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

class CScanner : public CListener<IScannerListener> {
public:
	inline CScanner(IScannerListener* listener);

	void Reset();

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
	void error(const TScannerErrorCodes errorCode, char c = '\0');

	TScannerState state;
	int localOffset;
	unsigned int stringCharCodeAcc;
};

inline CScanner::CScanner(IScannerListener* listener):
	CListener(listener)
{
	assert( HasListener() );

	Reset();
}

inline bool CScanner::identificatorIs(const std::string& toCompare) const
{
	return ( lexem == L_Identificator && toCompare == ToLower( lexemString ) );
}

} // end of namespace Refal2
