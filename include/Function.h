#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TRuntimeFunctionType {
	RFT_Empty,
	RFT_Embedded,
	RFT_Ordinary
};

class CRuntimeFunction {
public:
	TRuntimeFunctionType Type() const { return type; }
	bool IsEmpty() const { return ( Type() == RFT_Empty ); }
	bool IsEmbedded() const { return ( Type() == RFT_Embedded ); }
	bool IsOrdinary() const { return ( Type() == RFT_Ordinary ); }

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

typedef std::shared_ptr<CRuntimeFunction> CRuntimeFunctionPtr;
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

class COrdinaryFunction : public CRuntimeFunction {
public:
	COrdinaryFunction( const TOperationAddress _firstOperation,
			TRuntimeModuleId _runtimeModuleId ) :
		CRuntimeFunction( RFT_Ordinary ),
		firstOperation( _firstOperation ),
		runtimeModuleId( _runtimeModuleId )
	{
		assert( firstOperation != 0 );
	}

	TOperationAddress FirstOperation() const { return firstOperation; }
	TRuntimeModuleId RuntimeModuleId() const { return runtimeModuleId; }

private:
	TOperationAddress firstOperation;
	TRuntimeModuleId runtimeModuleId;
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

	void Print( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;

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
	PFT_Compiled
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

	bool IsEntry() const { return entry; }
	bool IsDeclared() const { return ( GetType() == PFT_Declared ); }
	bool IsDefined() const { return ( GetType() == PFT_Defined ); }
	bool IsOrdinary() const { return ( GetType() == PFT_Ordinary ); }
	bool IsEmpty() const { return ( GetType() == PFT_Empty ); }
	bool IsExternal() const { return ( GetType() == PFT_External ); }
	bool IsCompiled() const { return ( GetType() == PFT_Compiled ); }

	void SetDefined( const CToken& nameToken );
	void SetOrdinary( CRulePtr& firstRule );
	void SetEmpty();
	void SetEntry( const CToken& externalNameToken );
	void SetExternal( const CToken& externalNameToken );

	// Only for ordinary function
	void Compile( CFunctionCompiler& compiler );

	void Print( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;

private:
	TPreparatoryFunctionType type;
	bool entry;
	std::string name;
	CToken nameToken;
	std::string externalName;
	CToken externalNameToken;
	CRulePtr firstRule;
	TOperationAddress firstOperation;

	void setExternalName( const CToken& externalNameToken );

	CPreparatoryFunction( const CPreparatoryFunction& );
	CPreparatoryFunction& operator=( const CPreparatoryFunction& );
};

typedef std::unique_ptr<CPreparatoryFunction> CPreparatoryFunctionPtr;
typedef CDictionary<CPreparatoryFunctionPtr, std::string> CPreparatoryFunctions;

//-----------------------------------------------------------------------------

class CFunctionBuilder : public CVariablesBuilder {
protected:
	explicit CFunctionBuilder( IErrorHandler* errorHandler = nullptr );
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
	void AddVariable( TVariableTypeTag type, TVariableName name,
		CQualifier* qualifier = nullptr );
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
	static const char* getErrorMessage( TErrorCode errorCode );
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
