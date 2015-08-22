#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TRuntimeFunctionType {
	RFT_Empty,
	RFT_Embedded,
	RFT_External,
	RFT_Ordinary
};

class CRuntimeFunction {
public:
	TRuntimeFunctionType Type() const { return type; }

protected:
	CRuntimeFunction( TRuntimeFunctionType _type ) :
		type( _type )
	{
	}

private:
	TRuntimeFunctionType type;

	CRuntimeFunction( const CRuntimeFunction& );
	CRuntimeFunction& operator=( const CRuntimeFunction& );
};

typedef std::unique_ptr<CRuntimeFunction> CRuntimeFunctionPtr;
typedef CDictionary<CRuntimeFunctionPtr, std::string> CRuntimeFunctions;

//-----------------------------------------------------------------------------

class CEmptyFunction : public CRuntimeFunction {
public:
	CEmptyFunction() :
		CRuntimeFunction( RFT_Empty )
	{
	}
};

//-----------------------------------------------------------------------------

typedef bool ( *TEmbeddedFunctionPtr )();

class CEmbeddedFunction : public CRuntimeFunction {
public:
	CEmbeddedFunction( const TEmbeddedFunctionPtr _embeddedFunction ) :
		CRuntimeFunction( RFT_Embedded ),
		embeddedFunction( _embeddedFunction )
	{
		assert( embeddedFunction != 0 );
	}

	TEmbeddedFunctionPtr EmbeddedFunction() const { return embeddedFunction; }

private:
	TEmbeddedFunctionPtr embeddedFunction;
};

//-----------------------------------------------------------------------------

typedef int TOperation;

class COrdinaryFunction : public CRuntimeFunction {
public:
	COrdinaryFunction( const TOperation _firstOperation ) :
		CRuntimeFunction( RFT_Ordinary ),
		firstOperation( _firstOperation )
	{
		assert( firstOperation != 0 );
	}

	TOperation FirstOperation() const { return firstOperation; }

private:
	TOperation firstOperation;
};

//-----------------------------------------------------------------------------

typedef int TModuleId;

class CExternalFunction : public CRuntimeFunction {
public:
	CExternalFunction( TOperation _firstOperation, TModuleId _module ) :
		CRuntimeFunction( RFT_External ),
		firstOperation( _firstOperation ),
		module( _module )
	{
		assert( firstOperation != 0 );
	}

	TOperation FirstOperation() const { return firstOperation; }
	TModuleId Module() const { return module; }

private:
	TOperation firstOperation;
	TModuleId module;
};

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
	PFT_External,
	PFT_Compiled,
	PFT_Embedded
};

class CPreparatoryFunction {
public:
	CPreparatoryFunction( const CToken& name );

	TPreparatoryFunctionType GetType() const { return type; }

	const std::string& Name() const { return name; }
	const CToken& NameToken() const { return nameToken; }
	const std::string& ExternalName() const;
	const CToken& ExternalNameToken() const;
	const CRule* FirstRule() const;
	TOperationAddress FirstOperation() const;
	TEmbeddedFunctionPtr EmbeddedFunction() const;

	bool IsEntry() const { return entry; }
	bool IsDeclared() const { return ( GetType() == PFT_Declared ); }
	bool IsDefined() const { return ( GetType() == PFT_Defined ); }
	bool IsOrdinary() const { return ( GetType() == PFT_Ordinary ); }
	bool IsEmpty() const { return ( GetType() == PFT_Empty ); }
	bool IsExternal() const { return ( GetType() == PFT_External ); }
	bool IsCompiled() const { return ( GetType() == PFT_Compiled ); }
	bool IsEmbedded() const { return ( GetType() == PFT_Embedded ); }

	void SetDefined( const CToken& nameToken );
	void SetOrdinary( CRulePtr& firstRule );
	void SetEmpty();
	void SetEntry( const CToken& externalNameToken );
	void SetExternal( const CToken& externalNameToken );

	// Only for ordinary function
	void Compile( CFunctionCompiler& compiler );
	// Only for external function
	void Link( const CPreparatoryFunction& function );
	void SetEmbedded( const TEmbeddedFunctionPtr embeddedFunction );

	void Print( std::ostream& outputStream ) const;

private:
	TPreparatoryFunctionType type;
	bool entry;
	std::string name;
	CToken nameToken;
	std::string externalName;
	CToken externalNameToken;
	CRulePtr firstRule;
	TOperationAddress firstOperation;
	TEmbeddedFunctionPtr embeddedFunction;

	void setExternalName( const CToken& externalNameToken );

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
