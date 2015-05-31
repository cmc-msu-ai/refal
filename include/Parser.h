#pragma once

#include <map>
#include <string>
#include <Refal2.h>

namespace Refal2 {

enum TParserState {
	PS_Begin,
	PS_BeginIdent,
	PS_BeginBlank,
	PS_OnlyNewline,
	PS_WaitNewline,
	PS_BeginIdentBlank,
	PS_BeginIdentBlankS,
	
	PS_BeginEntry,
	PS_BeginEntryBlank,
	
	PS_BeginEmpty,
	PS_BeginEmptyComma,
	PS_BeginEmptyIdent,
	
	PS_BeginExtrn,
	PS_BeginExtrnComma,
	PS_BeginExtrnIdent,
	PS_BeginExtrnLeftParen,
	PS_BeginExtrnInnerIdent,
	PS_BeginExtrnRightParen,
	PS_BeginExtrnOnlyComma,
	
	PS_BeginProcessNamedQualifier,
	PS_ProcessNamedQualifier,
	PS_ProcessNamedQualifierAfterRightParen,
	PS_ProcessNamedQualifierAfterError,
	
	PS_BeginProcessVariableQualifier,
	PS_ProcessVariableQualifier,
	PS_ProcessVariableQualifierAfterRightParen,
	PS_ProcessVariableQualifierAfterError,
	
	PS_ProcessRuleDirection,
	PS_ProcessRule,
	PS_ProcessRuleAfterVariableType,
	PS_ProcessRuleAfterVariableQualifier,
	PS_ProcessRuleAfterLeftBracket
};

enum TParserErrorCode {
	PEC_LineShouldBeginWithIdentifierOrSpace,
	PEC_NewLineExpected,
	PEC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine,
	PEC_STUB
};

class IParserListener {
public:
	virtual void OnParserError( TParserErrorCode errorCode ) = 0;
};

class CParser :
	public CScanner,
	public CListener<IParserListener>
{
public:
	CParser( IParserListener* listener = 0 );
	void Reset();

	TLabel GetCurrentLabel() const { return currentFunction; }
	TLabel GetEntryLabel() const { return entryLabel; }
	
private:
	virtual void ProcessLexem();
	
	inline void error( TParserErrorCode errorCode );
	
	void processNamedQualifier( const bool afterRightParen = false );
	void processNamedQualifierAfterError();
	
	void addNamedQualifier();
	
	void processVariableQualifier( const bool afterRightParen = false );
	void processVariableQualifierAfterError();
	
	void addDeclarationOfFunction( const std::string& name );
	void addEndOfFunction();
	
	void addEmptyFunction( const std::string& name );
	void addEntryFunction( const std::string& name );
	void addExtrnFunction( const std::string& name,
		const std::string& standartName );
	
	TParserState state;
	int storedOffset;
	std::string storedName;
	
	TLabel entryLabel;

	TLabel currentFunction;
	TVariableType variableType;
	
	typedef std::map<std::string, CQualifier> CQualifierMap;
	
	CQualifier currentQualifier;
	CQualifierMap namedQualifiers;
	CQualifierBuilder qualifierBuilder;
};

inline void CParser::error( TParserErrorCode errorCode )
{
	SetErrors();
	if( CListener<IParserListener>::HasListener() ) {
		CListener<IParserListener>::GetListener()->OnParserError( errorCode );
	}
}

} // end of namespace Refal2
