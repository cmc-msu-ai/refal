#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const int InvalidVariableIndex = -1;

const TVariableType InvalidVariableType = '\0';
const TVariableType VariableTypeS = 's';
const TVariableType VariableTypeW = 'w';
const TVariableType VariableTypeV = 'v';
const TVariableType VariableTypeE = 'e';

class CVariable {
	friend class CVariables;
	friend class CVariablesBuilder;

public:
	TVariableType GetType() const { return type; }

	bool TypeIs( TVariableType type ) const;
	bool TypeIs( TVariableType tA, TVariableType tB ) const;

	CQualifier& GetQualifier() { return qualifier; }
	const CQualifier& GetQualifier() const { return qualifier; }

	static bool IsValidType( TVariableType type );
	static bool IsValidName( TVariableName name );

private:
	CVariable( TVariableType type, int originPosition, int countOfValues );

	TVariableType type;
	CQualifier qualifier;
	int originPosition;
	int topPosition;
	int position;
};

inline CVariable::CVariable( TVariableType t, int op, int countOfValues ):
	type( t ), originPosition( op ),
	topPosition( originPosition + countOfValues ), position( originPosition )
{
}

inline bool CVariable::TypeIs( TVariableType t ) const
{
	return ( type == t );
}

inline bool CVariable::TypeIs( TVariableType tA, TVariableType tB ) const
{
	return ( TypeIs(tA) || TypeIs(tB) );
}

inline bool CVariable::IsValidType( TVariableType type )
{
	return ( type == VariableTypeS || type == VariableTypeW ||
		type == VariableTypeV || type == VariableTypeE );
}

inline bool CVariable::IsValidName( TVariableName name )
{
	return ( ::isalnum( name ) != 0 );
}

//-----------------------------------------------------------------------------

class CVariables {
	friend class CVariablesBuilder;

public:
	CVariables();
	~CVariables();

	void Reset();

	void Swap( CVariables& swapWith );
	void Move( CVariables& moveTo );

	CVariable& GetVariable( TVariableIndex variableIndex );
	const CVariable& GetVariable( TVariableIndex variableIndex ) const;

	bool IsValidVariableIndex( TVariableIndex variableIndex ) const;

	bool IsFull( TVariableIndex variableIndex ) const;
	// true if variable have one or more value
	bool IsSet( TVariableIndex variableIndex ) const;
	void Set( TVariableIndex variableIndex, TTableIndex tableIndex );
	TTableIndex GetMainValue( TVariableIndex variableIndex ) const;
	// true if tableIndex is fresh
	bool Get( TVariableIndex variableIndex, TTableIndex& tableIndex );

private:
	CVariables( const CVariables& );
	CVariables& operator=( const CVariables& );

	void allocVariablesValues();

	CVariable* variables;
	int variablesSize;
	TTableIndex* variablesValues;
	int variablesValuesSize;
};

inline CVariables::CVariables():
	variables(0), variablesSize(0), variablesValues(0), variablesValuesSize(0)
{
}

inline CVariables::~CVariables()
{
	Reset();
}

inline void CVariables::Move( CVariables& moveTo )
{
	if( this != &moveTo ) {
		Swap( moveTo );
		Reset();
	}
}

inline CVariable& CVariables::GetVariable( TVariableIndex variableIndex )
{
	assert( IsValidVariableIndex( variableIndex ) );
	return variables[variableIndex];
}

inline const CVariable& CVariables::GetVariable(
	TVariableIndex variableIndex ) const
{
	assert( IsValidVariableIndex( variableIndex ) );
	return variables[variableIndex];
}

inline bool CVariables::IsValidVariableIndex(
	TVariableIndex variableIndex ) const
{
	return ( variableIndex >= 0 && variableIndex < variablesSize );
}

inline void CVariables::allocVariablesValues()
{
	if( variablesValues == 0 ) {
		assert( variablesValuesSize > 0 );
		variablesValues = new TTableIndex[variablesValuesSize];
	}
}

//-----------------------------------------------------------------------------

const int VariablesInfoSize = 128;

class CVariablesBuilder : public CErrorsHelper {
public:
	explicit CVariablesBuilder( IErrorHandler* errorProcessor = 0 );

	void Reset();
	void Export( CVariables& variables );

	TVariableIndex AddLeft( TVariableName name, TVariableType type,
		CQualifier* qualifier = 0 );
	TVariableIndex AddRight( TVariableName name, TVariableType type,
		CQualifier* qualifier = 0 );

private:
	bool checkName( TVariableName name );
	bool checkType( TVariableType type );
	// processing errors
	enum TErrorCode {
		EC_InvalidVariableName,
		EC_NoSuchTypeOfVariable,
		EC_TypeOfVariableDoesNotMatch,
		EC_NoSuchVariableInLeftPart
	};
	void error( TErrorCode errorCode );

	struct CVariableInfo {
		TVariableIndex name;
		TVariableType type;
		int countLeft;
		int countRight;
		CQualifier qualifier;
	};

	CVariableInfo variables[VariablesInfoSize];
	int countOfVariables;

	CVariablesBuilder( const CVariablesBuilder& );
	CVariablesBuilder& operator=( const CVariablesBuilder& );
};

//-----------------------------------------------------------------------------

} // end of namespace refal2
