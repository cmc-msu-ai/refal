#pragma once

#include <stack>
#include <Refal2.h>

namespace Refal2 {

struct CFunctionRule;

void PrintRule(const CFunctionRule* rule);
void PrintFunction(const CFunctionRule* firstRule);

struct CFunctionRule {
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
	FS_Compiled
};

class CFunctionCompiler;

class CFunction {
	friend class CFunctionCompiler;
	
public:
	CFunction(TFunctionState state = FS_Declared):
		functionState(state), firstRule(0) {}

	TFunctionState GetFunctionState() const { return functionState; }
	bool IsDeclared() const { return ( functionState == FS_Declared ); }
	bool IsDefined() const { return ( functionState == FS_Declared ); }
	bool IsParsed() const { return ( functionState == FS_Parsed ); }
	bool IsCompiled() const { return ( functionState == FS_Compiled ); }
	
	inline void SetDefined();
	inline void SetParsed(CFunctionRule** firstRule);
	
/*private:*/
	TFunctionState functionState;
	union {
		CFunctionRule* firstRule;
		/*COperation* firstOperation;*/
	};
};

inline void CFunction::SetDefined()
{
	assert( functionState == FS_Declared );
	
	functionState = FS_Defined;
}

inline void CFunction::SetParsed(CFunctionRule** _firstRule)
{
	assert( functionState == FS_Defined );
	
	firstRule = *_firstRule;
	*_firstRule = 0;
}

enum TFunctionBuilderState {
	FBS_Direction,
	FBS_Left,
	FBS_Right
};

enum TFunctionBuilderErrorCodes {
	FBEC_ThereAreNoRulesInFunction,
	FBEC_IllegalLeftBracketInLeftPart,
	FBEC_IllegalRightBracketInLeftPart,
	FBEC_RightParenDoesNotMatchLeftParen,
	FBEC_RightBracketDoesNotMatchLeftBracket,
	FBEC_UnclosedLeftParenInLeftPart,
	FBEC_UnclosedLeftParenInRightPart,
	FBEC_UnclosedLeftBracketInRightPart,
	FBEC_IllegalQualifierInRightPart
};

class IFunctionBuilderListener : public IVariablesBuilderListener {
public:
	virtual void OnFunctionBuilderError(const TFunctionBuilderErrorCodes) = 0;
};

class CFunctionBuilder : public CListener<IFunctionBuilderListener> {
public:
	explicit CFunctionBuilder(IFunctionBuilderListener* listener);
	~CFunctionBuilder() { Reset(); }
	
	virtual void OnVariablesBuilderError(const TVariablesBuilderErrorCodes);
	
	bool HasErrors() const { return errors; }
	void SetErrors();
	inline void ResetErrors();
	
	TFunctionBuilderState GetState() const { return state; }
	bool IsDirectionState() const { return ( state == FBS_Direction ); }
	bool IsLeftState() const { return ( state == FBS_Left ); }
	bool IsRightState() const { return ( state == FBS_Right ); }
	
	const CUnitNode* GetLastAddedUnit() const { return acc.GetLast(); }
	
	void Reset();
	void Export(CFunction* function);
	
	void AddDirection(bool _isRightDirection = false);
	void AddEndOfLeft();
	void AddEndOfRight();
	inline void AddChar(TChar c);
	inline void AddLabel(TLabel label);
	inline void AddNumber(TNumber number);
	void AddLeftVariable(TVariableType type, TVariableName name,
		CQualifier* qualifier = 0);
	void AddRightVariable(TVariableType type, TVariableName name);
	void AddLeftParen();
	void AddRightParen();
	void AddLeftBracket();
	void AddRightBracket();
	
private:
	CFunctionBuilder(const CFunctionBuilder&);
	CFunctionBuilder& operator=(const CFunctionBuilder&);
	
	inline void emptyStack();	
	inline void error(const TFunctionBuilderErrorCodes errorCode);
	void emptyRules();
	void addRule();
	
	TFunctionBuilderState state;
	bool errors;
	bool isRightDirection;
	CUnitList acc;
	CUnitList leftPart;
	CFunctionRule* firstRule;
	CFunctionRule* lastRule;
	CVariablesBuilder variablesBuilder;
	std::stack<CUnitNode*> balanceStack;
};

inline void CFunctionBuilder::ResetErrors()
{
	errors = false;
	Reset();
}

inline void CFunctionBuilder::emptyStack()
{
	while( !balanceStack.empty() ) {
		balanceStack.pop();
	}
}
	
inline void CFunctionBuilder::error(const TFunctionBuilderErrorCodes errorCode)
{
	SetErrors();
	GetListener()->OnFunctionBuilderError( errorCode );
}

inline void CFunctionBuilder::AddChar(TChar c)
{
	if( !HasErrors() ) {
		acc.AppendChar( c );
	}
}

inline void CFunctionBuilder::AddLabel(TLabel label)
{
	if( !HasErrors() ) {
		acc.AppendLabel( label );
	}
}

inline void CFunctionBuilder::AddNumber(TNumber number)
{
	if( !HasErrors() ) {
		acc.AppendNumber( number );
	}
}

} // end of namespace refal2
