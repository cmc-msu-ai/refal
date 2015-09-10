#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const int InvalidVariableIndex = -1;

enum TVariableType {
	VT_None,
	VT_S,
	VT_W,
	VT_V,
	VT_E
};

class CVariable {
	friend class CVariables;
	friend class CVariablesBuilder;

public:
	TVariableType GetType() const { return type; }

	bool TypeIs( TVariableType type ) const;
	bool TypeIs( TVariableType t1, TVariableType t2 ) const;

	CQualifier& GetQualifier() { return qualifier; }
	const CQualifier& GetQualifier() const { return qualifier; }

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

inline bool CVariable::TypeIs( TVariableType t1, TVariableType t2 ) const
{
	return ( TypeIs( t1 ) || TypeIs( t2 ) );
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

const int VariableNameToIndexSize = 256;
const int MaxNumberOfVariables = 64;

class CVariablesBuilder : public CErrorsHelper {
protected:
	explicit CVariablesBuilder( IErrorHandler* errorHandler = nullptr );

	void Reset();
	void Export( CVariables& variables );

	TVariableIndex AddLeft( const TVariableName name,
		const TVariableTypeTag type );
	TVariableIndex AddRight( const TVariableName name,
		const TVariableTypeTag type );
	void AddQualifier( const TVariableIndex variableIndex,
		CQualifier& qualifier );

private:
	struct CVariableData {
		TVariableType Type;
		int CountLeft;
		int CountRight;
		CQualifier Qualifier;
	};

	TVariableIndex variableNameToIndex[VariableNameToIndexSize];
	CVariableData variables[MaxNumberOfVariables];
	int firstFreeVariableIndex;

	// disable copy constructor and operator=
	CVariablesBuilder( const CVariablesBuilder& );
	CVariablesBuilder& operator=( const CVariablesBuilder& );

	bool checkName( const TVariableName name );
	TVariableType checkTypeTag( const TVariableTypeTag type,
		const CQualifier** defaultQualifierForTag = 0 );
	// processing errors
	void error( const std::string& errorMessage );
	void errorTypesNotMatched( char name, TVariableType type );
	void warningObsoleteVariableType( char type, const char* replacement );
};

//-----------------------------------------------------------------------------

} // end of namespace refal2
