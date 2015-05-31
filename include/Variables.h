#pragma once

#include <Refal2.h>

namespace Refal2 {

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

inline CVariable::CVariable( TVariableType _type, int _originPosition,
		int countOfValues ):
	type( _type ),
	originPosition( _originPosition ),
	topPosition( originPosition + countOfValues ),
	position( originPosition )
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

class CVariables {
	friend class CVariablesBuilder;
	
public:
	inline CVariables();
	~CVariables() { Reset(); }

	void Reset();	
	void Import( CVariablesBuilder& variablesBuiler );

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

enum TVariablesBuilderErrorCode {
	VBEC_InvalidVatiableName,
	VBEC_NoSuchTypeOfVariable,
	VBEC_TypeOfVariableDoesNotMatch,
	VBEC_NoSuchVariableInLeftPart
};

class IVariablesBuilderListener {
public:
	virtual void OnVariablesBuilderError( TVariablesBuilderErrorCode ) = 0;
};

const int VariablesInfoSize = 128;

class CVariablesBuilder :
	public CErrors,
	public CListener<IVariablesBuilderListener>
{
	friend class CVariables;

public:
	explicit CVariablesBuilder( IVariablesBuilderListener* listener = 0 );
	
	void Reset();
	void Export( CVariables& variables );
	
	TVariableIndex AddLeft( TVariableName name, TVariableType type,
		CQualifier* qualifier = 0 );
	TVariableIndex AddRight( TVariableName name, TVariableType type,
		CQualifier* qualifier = 0 );
	
private:
	CVariablesBuilder( const CVariablesBuilder& );
	CVariablesBuilder& operator=( const CVariablesBuilder& );

	bool checkName( TVariableName name );
	bool checkType( TVariableType type );
	void error( TVariablesBuilderErrorCode errorCode );

	struct CVariableInfo {
		TVariableIndex name;
		TVariableType type;
		int countLeft;
		int countRight;
		CQualifier qualifier;
	};

	CVariableInfo variables[VariablesInfoSize];
	int countOfVariables;
};

inline void CVariablesBuilder::Export( CVariables& variables )
{
	variables.Import( *this );
	Reset();
}

inline bool CVariablesBuilder::checkName( TVariableName name )
{
	if( CVariable::IsValidName( name ) ) {
		return true;
	} else {
		error( VBEC_InvalidVatiableName );
		return false;
	}
}

inline bool CVariablesBuilder::checkType( TVariableType type )
{
	if( CVariable::IsValidType( type ) ) {
		return true;
	} else {
		error( VBEC_NoSuchTypeOfVariable );
		return false;
	}
}


inline void CVariablesBuilder::error( TVariablesBuilderErrorCode errorCode )
{
	SetErrors();
	if( HasListener() ) {
		GetListener()->OnVariablesBuilderError( errorCode );
	}
}

} // end of namespace refal2
