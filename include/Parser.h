#pragma once

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
	
	PS_ProcessQualifier,
	PS_ProcessRule
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
	void processRule();
	void processQualifier();

	void addDeclarationOfFunction(const std::string& name);
	void addEndOfFunction();

	void addDeclarationOfQualifier(const std::string& name);

	void addEmptyFunction(const std::string& name);
	void addEntryFunction(const std::string& name);
	void addExtrnFunction(const std::string& name, const std::string& oldName);

	TParserState state;
	int storedOffset;
	std::string storedName;
	CFunctionBuilder functionBuilder;
	//CFunction* currentFunction;
	//CQualifier* currentQualifier;
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
