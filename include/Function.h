#pragma once

#include <stack>
#include <Refal2.h>

namespace Refal2 {

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

enum TFunctionBuilderErrorCode {
	FBEC_ThereAreNoRulesInFunction,
	FBEC_IllegalLeftBracketInLeftPart,
	FBEC_IllegalRightBracketInLeftPart,
	FBEC_RightParenDoesNotMatchLeftParen,
	FBEC_RightBracketDoesNotMatchLeftBracket,
	FBEC_UnclosedLeftParenInLeftPart,
	FBEC_UnclosedLeftParenInRightPart,
	FBEC_UnclosedLeftBracketInRightPart,
	FBEC_ThereAreMultiplePartsSeparatorInRules,
	FBEC_ThereAreNoPartsSeparatorInRules
};

class IFunctionBuilderListener {
public:
	virtual void OnFunctionBuilderError( TFunctionBuilderErrorCode ) = 0;
};

class CFunctionBuilder :
	public CVariablesBuilder,
	public CListener<IFunctionBuilderListener>
{
public:
	explicit CFunctionBuilder( IFunctionBuilderListener* listener = 0 );
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
	CFunctionBuilder( const CFunctionBuilder& );
	CFunctionBuilder& operator=( const CFunctionBuilder& );

	virtual void OnErrors();
	
	void emptyStack();
	void error( TFunctionBuilderErrorCode errorCode );
	void emptyRules();
	void addRule();
	
	bool isProcessRightPart;
	bool isRightDirection;
	CUnitList acc;
	CUnitList leftPart;
	CFunctionRule* firstRule;
	CFunctionRule* lastRule;
	std::stack<CUnitNode*> balanceStack;
};

inline CFunctionBuilder::~CFunctionBuilder()
{
	Reset();
}

inline void CFunctionBuilder::emptyStack()
{
	while( !balanceStack.empty() ) {
		balanceStack.pop();
	}
}
	
inline void CFunctionBuilder::error( TFunctionBuilderErrorCode errorCode )
{
	SetErrors();
	if( CListener<IFunctionBuilderListener>::HasListener() ) {
		CListener<IFunctionBuilderListener>::GetListener()->
			OnFunctionBuilderError( errorCode );
	}
}

inline void CFunctionBuilder::AddChar( TChar c )
{
	if( !HasErrors() ) {
		acc.AppendChar( c );
	}
}

inline void CFunctionBuilder::AddLabel( TLabel label )
{
	if( !HasErrors() ) {
		acc.AppendLabel( label );
	}
}

inline void CFunctionBuilder::AddNumber( TNumber number )
{
	if( !HasErrors() ) {
		acc.AppendNumber( number );
	}
}

} // end of namespace refal2
