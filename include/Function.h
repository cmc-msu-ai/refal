#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CRule {
public:
	CUnitList Left;
	CUnitList Right;
	bool RightMatching;
	CVariables Variables;
	std::unique_ptr<CRule> NextRule;

	CRule(): RightMatching( false ) {}

	void Compile( CFunctionCompiler& compiler ) const;

	void Print( std::ostream& outputStream ) const;

private:
	CRule( const CRule& );
	CRule& operator=( const CRule& );
};

typedef std::unique_ptr<CRule> CRulePtr;

//-----------------------------------------------------------------------------

enum TPreparatoryFunctionType {
	PFT_Declared,
	PFT_Defined,
	PFT_Ordinary,
	PFT_Empty,
	PFT_External
};

class CPreparatoryFunction {
public:
	CPreparatoryFunction( const CToken& name );

	TPreparatoryFunctionType GetType() const { return type; }
	const CToken& GetName() const { return name; }
	const CToken& GetExternalName() const { return externalName; }
	const CRule* GetFirstRule() const;

	bool IsEntry() const { return entry; }
	bool IsDeclared() const { return ( GetType() == PFT_Declared ); }
	bool IsDefined() const { return ( GetType() == PFT_Defined ); }
	bool IsOrdinary() const { return ( GetType() == PFT_Ordinary ); }
	bool IsEmpty() const { return ( GetType() == PFT_Empty ); }
	bool IsExternal() const { return ( GetType() == PFT_External ); }

	void SetDefined( const CToken& name );
	void SetOrdinary( CRulePtr& firstRule );
	void SetEmpty();
	void SetEntry( const CToken& externalName );
	void SetExternal( const CToken& externalName );

	void Compile( CFunctionCompiler& compiler ) const;

	void Print( std::ostream& outputStream ) const;

private:
	TPreparatoryFunctionType type;
	bool entry;
	CToken name;
	CToken externalName;
	CRulePtr firstRule;

	CPreparatoryFunction( const CPreparatoryFunction& );
	CPreparatoryFunction& operator=( const CPreparatoryFunction& );
};

typedef std::unique_ptr<CPreparatoryFunction> CPreparatoryFunctionPtr;
typedef CDictionary<CPreparatoryFunctionPtr, std::string> CPreparatoryFunctions;

//-----------------------------------------------------------------------------

class CFunctionBuilder : public CVariablesBuilder {
protected:
	explicit CFunctionBuilder( IErrorHandler* errorHandler = 0 );
	~CFunctionBuilder();

	void Reset();
	void Export( CRulePtr& firstRule );

	bool IsProcessRightPart() const { return isProcessRightPart; }

	void SetRightDirection();
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
	CRulePtr firstRule;
	CRule* lastRule;
	std::stack<CUnitNode*> balanceStack;
	// processing errors
	enum TErrorCode {
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
