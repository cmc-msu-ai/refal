#pragma once

#include <Refal2.h>
#include <stack>

namespace Refal2 {

//-----------------------------------------------------------------------------

void PrintRule(const CFunctionRule& rule);
void PrintFunction(const CFunction& function);

struct CFunctionRule {
	friend class CFunctionBuilder;
	friend class CFunctionCompiler;
	friend void PrintRule(const CFunctionRule& rule);
	friend void PrintFunction(const CFunction& function);
	
private:
	CUnitList leftPart;
	CUnitList rightPart;
	CVariables variables;
	bool isRightDirection;
	CFunctionRule* nextRule;
	
	CFunctionRule(bool _isRightDirection):
		isRightDirection(_isRightDirection), nextRule(0) {}
};

//-----------------------------------------------------------------------------

enum TFunctionState {
	FS_Declared,
	FS_Defined,
	FS_Parsed,
	FS_Compiled,
	FS_Empty,
	FS_External
};

typedef bool (*TExternalFunction)( CUnitList& argument, std::string& errorText );

class CFunction {
	friend class CFunctionCompiler;
	friend class COperationsExecuter;
	friend void PrintFunction( const CFunction& function );

public:
	CFunction( TFunctionState state = FS_Declared ):
		functionState( state ), firstRule( 0 ) {}

	TFunctionState GetFunctionState() const { return functionState; }
	bool IsDeclared() const { return ( functionState == FS_Declared ); }
	bool IsDefined() const { return ( functionState == FS_Defined ); }
	bool IsParsed() const { return ( functionState == FS_Parsed ); }
	bool IsCompiled() const { return ( functionState == FS_Compiled ); }
	bool IsEmpty() const { return ( functionState == FS_Empty ); }
	bool IsExternal() const { return ( functionState == FS_External ); }
	
	void SetDefined();
	void SetParsed( CFunctionRule** firstRule );
	void SetCompiled( COperation* operation );
	void SetEmpty();
	void SetExternal( TExternalFunction externalFunction );
	
private:
	TFunctionState functionState;
	union {
		CFunctionRule* firstRule;
		COperation* firstOperation;
		TExternalFunction externalFunction;
	};
};

inline void CFunction::SetDefined()
{
	assert( functionState == FS_Declared );
	functionState = FS_Defined;
}

inline void CFunction::SetParsed( CFunctionRule** _firstRule )
{
	assert( functionState == FS_Defined );
	firstRule = *_firstRule;
	*_firstRule = 0;
	functionState = FS_Parsed;
}

inline void CFunction::SetCompiled( COperation* operation )
{
	assert( functionState == FS_Parsed );
	firstOperation = operation;
	functionState = FS_Compiled;
}

inline void CFunction::SetEmpty()
{
	assert( functionState == FS_Declared );
	functionState = FS_Empty;
}

inline void CFunction::SetExternal( TExternalFunction _externalFunction )
{
	assert( functionState == FS_Declared );
	functionState = FS_External;
	externalFunction = _externalFunction;
}

//-----------------------------------------------------------------------------

class CFunctionBuilder : public CVariablesBuilder {
public:
	explicit CFunctionBuilder( IErrorHandler* errorProcessor = 0 );
	~CFunctionBuilder();

	void Reset();
	void Export( CFunction& function );

	bool IsProcessRightPart() const { return isProcessRightPart; }

	void SetRightDirection() { isRightDirection = true; }
	void AddEndOfLeft();
	void AddEndOfRight();
	void AddChar( TChar c );
	void AddLabel( TLabel label );
	void AddNumber( TNumber number );
	void AddVariable( TVariableType type, TVariableName name,
		CQualifier* qualifier = 0 );
	void AddLeftParen();
	void AddRightParen();
	void AddLeftBracket();
	void AddRightBracket();

private:
	bool isProcessRightPart;
	bool isRightDirection;
	CUnitList acc;
	CUnitList leftPart;
	CFunctionRule* firstRule;
	CFunctionRule* lastRule;
	std::stack<CUnitNode*> balanceStack;
	// processing errors
	enum TErrorCode {
		EC_ThereAreNoRulesInFunction,
		EC_IllegalLeftBracketInLeftPart,
		EC_IllegalRightBracketInLeftPart,
		EC_RightParenDoesNotMatchLeftParen,
		EC_RightBracketDoesNotMatchLeftBracket,
		EC_UnclosedLeftParenInLeftPart,
		EC_UnclosedLeftParenInRightPart,
		EC_UnclosedLeftBracketInRightPart,
		EC_ThereAreMultiplePartsSeparatorInRules,
		EC_ThereAreNoPartsSeparatorInRules
	};
	void error( TErrorCode errorCode );
	// auxiliary functions
	void emptyStack();
	void emptyRules();
	void addRule();

	CFunctionBuilder( const CFunctionBuilder& );
	CFunctionBuilder& operator=( const CFunctionBuilder& );
};

//-----------------------------------------------------------------------------

} // end of namespace refal2
