#pragma once

#include <Refal2.h>

namespace Refal2 {

const int InvalidVariableIndex = -1;

class CVariables;
class CVariablesBuilder;

const TVariableType InvalidVariableType = '\0';

class CVariable {
	friend class CVariables;
	friend class CVariablesBuilder;

public:
	TVariableType GetType() const { return type; }

	inline bool TypeIs(const TVariableType type) const;
	inline bool TypeIs(const TVariableType tA, const TVariableType tB) const;

	CQualifier* GetQualifier() { return &qualifier; }
	const CQualifier& GetQualifier() const { return qualifier; }

	inline static bool IsValidType(const TVariableType type);
	inline static bool IsValidName(const TVariableName name);

private:
	inline CVariable(const TVariableType _type, const int _originPosition,
		const int _topPosition);

	TVariableType type;
	CQualifier qualifier;
	int originPosition;
	int topPosition;
	int position;
};

inline CVariable::CVariable(const TVariableType _type,
		const int _originPosition, const int countOfValues):
	type( _type ),
	originPosition( _originPosition ),
	topPosition( originPosition + countOfValues ),
	position( originPosition )
{
}

inline bool CVariable::TypeIs(const TVariableType t) const
{
	return ( type == t );
}

inline bool CVariable::TypeIs(const TVariableType tA,
	const TVariableType tB) const
{
	return ( TypeIs(tA) || TypeIs(tB) );
}

inline bool CVariable::IsValidType(const TVariableType type)
{
	return ( type == 's' || type == 'w' || type == 'v' || type == 'e' );
}

inline bool CVariable::IsValidName(const TVariableName name)
{
	return ( ::isalnum( name ) != 0 );
}

class CVariables {
	friend class CVariablesBuilder;
	
public:
	inline CVariables();
	~CVariables() { Reset(); }
	void Reset();
	
	void Import(CVariablesBuilder* variablesBuiler);

	void Swap(CVariables* swapWith);
	inline void Move(CVariables* moveTo);

	inline CVariable* GetVariable(const TVariableIndex variableIndex);
	inline const CVariable& GetVariable(
		const TVariableIndex variableIndex) const;

	inline bool IsValidVariableIndex(const TVariableIndex variableIndex) const;

	// true if variable have one or more value
	bool IsSet(const TVariableIndex variableIndex) const;
	void Set(const TVariableIndex variableIndex, const TTableIndex tableIndex);
	// true if tableIndex is fresh
	bool Get(const TVariableIndex variableIndex, TTableIndex* tableIndex);
	
private:
	CVariables(const CVariables&);
	CVariables& operator=(const CVariables&);
	
	inline void allocVariablesValues();

	CVariable* variables;
	int variablesSize;
	TTableIndex* variablesValues;
	int variablesValuesSize;
};

inline CVariables::CVariables():
	variables(0), variablesSize(0),
	variablesValues(0), variablesValuesSize(0)
{
}

inline void CVariables::Move(CVariables* moveTo)
{
	moveTo->Reset();
	Swap( moveTo );
}

inline CVariable* CVariables::GetVariable(const TVariableIndex variableIndex)
{
	assert( IsValidVariableIndex( variableIndex ) );

	return &variables[variableIndex];
}

inline const CVariable& CVariables::GetVariable(
	const TVariableIndex variableIndex) const
{
	assert( IsValidVariableIndex( variableIndex ) );

	return variables[variableIndex];
}

inline bool CVariables::IsValidVariableIndex(
	const TVariableIndex variableIndex) const
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

enum TVariablesBuilderErrorCodes {
	VBEC_InvalidVatiableName,
	VBEC_NoSuchTypeOfVariable,
	VBEC_TypeOfVariableDoesNotMatch,
	VBEC_NoSuchVariableInLeftPart
};

class IVariablesBuilderListener {
public:
	virtual void OnVariablesBuilderError(const TVariablesBuilderErrorCodes) = 0;
};

class CVariablesBuilder : public CListener<IVariablesBuilderListener> {
	friend class CVariables;

public:
	explicit CVariablesBuilder(IVariablesBuilderListener* listener);
	
	void Reset();
	inline void Export(CVariables* variables);
	
	TVariableIndex AddLeft(TVariableName name, TVariableType type,
		CQualifier* qualifier = 0);
	TVariableIndex AddRight(TVariableName name, TVariableType type);
	
private:
	CVariablesBuilder(const CVariablesBuilder&);
	CVariablesBuilder& operator=(const CVariablesBuilder&);

	inline bool checkName(const TVariableName name);
	inline bool checkType(const TVariableType type);
	inline void error(const TVariablesBuilderErrorCodes errorCode);

	static const int variablesInfoSize = 128;

	struct CVariableInfo {
		TVariableIndex name;
		TVariableType type;
		int countLeft;
		int countRight;
		CQualifier qualifier;
	};

	CVariableInfo variables[variablesInfoSize];
	int countOfVariables;
};

inline void CVariablesBuilder::Export(CVariables* variables)
{
	variables->Import( this );
	Reset();
}

inline bool CVariablesBuilder::checkName(const TVariableName name)
{
	if( CVariable::IsValidName( name ) ) {
		return true;
	} else {
		error( VBEC_InvalidVatiableName );
		return false;
	}
}

inline bool CVariablesBuilder::checkType(const TVariableType type)
{
	if( CVariable::IsValidType( type ) ) {
		return true;
	} else {
		error( VBEC_NoSuchTypeOfVariable );
		return false;
	}
}


inline void CVariablesBuilder::error(const TVariablesBuilderErrorCodes errorCode)
{
	GetListener()->OnVariablesBuilderError( errorCode );
}

} // end of namespace refal2
