#pragma once

#include <stdio.h>
#include <Refal2.h>

namespace Refal2 {

class CQualifier;

const int InvalidVariableIndex = -1;

class CVariablesBuilder;

class CVariables {
	friend class CVariablesBuilder;
	
public:
	CVariables(): variables(0), capacity(0), values(0) {}
	~CVariables() { Reset(); }
	void Reset();
	
	void Move(CVariables* variablesTo);
	
	inline TVariableType GetType(TVariableIndex variable) const;
	inline bool TypeIs(TVariableIndex variable, TVariableType type) const;
	inline bool TypeIs(TVariableIndex variable, TVariableType type1,
		TVariableType type2) const;
	
	bool HaveValue(TVariableIndex variable) const { return false; }
		
	void Set(TVariableIndex variable, int value);
	int Get(TVariableIndex variable, TVariableType* type, bool* doMove);
	
private:
	CVariables(const CVariables&);
	CVariables& operator=(const CVariables&);
	
	void setCapacity(int _capacity) { capacity = _capacity; }
	void allocVariables(int countOfVariables);
	void allocValues();	
	
	struct CVariable {
		TVariableType type;
		int initialOffset;
		int currentOffset;
		//CQualifier qualifier;
	};
	
	CVariable* variables;
	int capacity;
	int* values;
};

inline TVariableType CVariables::GetType(TVariableIndex variable) const
{
	return variables[variable].type;
}

inline bool CVariables::TypeIs(TVariableIndex variable, TVariableType type) const
{
	return ( variables[variable].type == type );
}

inline bool CVariables::TypeIs(TVariableIndex variable, TVariableType type1,
	TVariableType type2) const
{
	return ( TypeIs(variable, type1) || TypeIs(variable, type2) );
}

enum TVariablesBuilderErrorCodes {
	VBEC_NoSuchTypeOfVariable,
	VBEC_TypeOfVariableDoesNotMatch,
	VBEC_NoSuchVariableInLeftPart
};

class IVariablesBuilderListener {
public:
	virtual void OnVariablesBuilderError(const TVariablesBuilderErrorCodes) = 0;
};

class CVariablesBuilder : public CListener<IVariablesBuilderListener> {
public:
	explicit CVariablesBuilder(IVariablesBuilderListener* listener);
	
	void Reset();
	void Export(CVariables* variables);
	
	TVariableIndex AddLeft(TVariableName name, TVariableType type,
		CQualifier* qualifier = 0);
	TVariableIndex AddRight(TVariableName name, TVariableType type);
		
private:
	CVariablesBuilder(const CVariablesBuilder&);
	CVariablesBuilder& operator=(const CVariablesBuilder&);
	
	inline void error(const TVariablesBuilderErrorCodes errorCode);

	static const int variablesSize = 128;

	struct CVariableInfo {
		TVariableIndex name;
		TVariableType type;
		int count;
		//CQualifier qualifier;
	};

	CVariableInfo variables[variablesSize];
	int countOfVariables;
};

inline void CVariablesBuilder::error(const TVariablesBuilderErrorCodes errorCode)
{
	GetListener()->OnVariablesBuilderError( errorCode );
}

} // end of namespace refal2
