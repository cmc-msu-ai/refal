#pragma once

#include <Refal2.h>
#include <map>
#include <string>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CParser :	public CErrorsHelper {
public:
	CParser( IErrorHandler* errorProcessor = 0 );
	void Reset();

	TLabel GetCurrentLabel() const { return currentFunction; }
	TLabel GetEntryLabel() const { return entryLabel; }

protected:
	CToken token;

	void AddToken();

private:
	CRuleParser ruleParser;
	CQualifierParser qualifierParser;
	CFunctionBuilder functionBuilder;
	// processing errors
	enum TErrorCode {
		EC_LineShouldBeginWithIdentifierOrSpace,
		EC_NewLineExpected,
		EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine,
		EC_STUB
	};
	void error( TErrorCode errorCode );	
	// parsing
	enum TState {
		S_Initial,
		S_IgnoreLine,
		S_Word,
		S_WordBlank,
		S_WordBlankS,
		S_Blank,
		S_Directive,
		S_Qualifier,
		S_RuleDirection,
		S_Rule
	};
	TState state;
	void parsingInitial();
	void parsingIgnoreLine();
	void parsingWord();
	void parsingWordBlank();
	void parsingWordBlankS();
	void parsingBlank();
	void parsingDirective();
	void parsingQualifier();
	void parsingRuleDirection();
	void parsingRule();

	bool wordIs( const std::string& word ) const;

	void addNamedQualifier();

	void addDeclarationOfFunction( const std::string& name );
	void addEndOfFunction();
	
	void addEmptyFunction( const std::string& name );
	void addEntryFunction( const std::string& name );
	void addExtrnFunction( const std::string& name,
		const std::string& standartName );

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

//-----------------------------------------------------------------------------

} // end of namespace Refal2
