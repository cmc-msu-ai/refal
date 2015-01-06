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
	PS_ProcessLeftPartOfRule,
	PS_ProcessLeftPartOfRuleAfterVariableType,
	PS_ProcessLeftPartOfRuleAfterVariableQualifier,
	PS_ProcessRightPartOfRule,
	PS_ProcessRightPartOfRuleAfterLeftBracket
};

enum TParserErrorCodes {
};

class IParserListener {
public:
	virtual void OnParserError(const TParserErrorCodes errorCode) = 0;
};

class CParser : public CScanner, public CListener<IParserListener> {
public:
	inline CParser(IParserListener* listener);

	void Reset();

protected:
	virtual void ProcessLexem();

private:
	void processNamedQualifier(const bool afterRightParen = false);
	void processNamedQualifierAfterError();

	void addNamedQualifier();

	void processVariableQualifier(const bool afterRightParen = false);
	void processVariableQualifierAfterError();

	void addDeclarationOfFunction(const std::string& name);
	void addEndOfFunction();	

	void addEmptyFunction(const std::string& name);
	void addEntryFunction(const std::string& name);
	void addExtrnFunction(const std::string& name, const std::string& oldName);

	TParserState state;
	int storedOffset;
	std::string storedName;

	TLabel currentFunction;
	TVariableType variableType;
	CFunctionBuilder functionBuilder;

	typedef std::map<std::string, CQualifier> TQualifierMap;

	CQualifier currentQualifier;
	TQualifierMap namedQualifiers;
	CQualifierBuilder qualifierBuilder;
};

inline CParser::CParser(IParserListener* listener):
	CScanner( dynamic_cast<IScannerListener*>(listener) ),
	CListener<IParserListener>(listener),
	functionBuilder( dynamic_cast<IFunctionBuilderListener*>(listener) )
{
	assert( CListener<IParserListener>::HasListener() );

	Reset();
}

} // end of namespace Refal2