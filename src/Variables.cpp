#include <Refal2.h>

namespace Refal2 {
	
void CVariables::Reset()
{
	if( variables != 0 ) {
		delete[] variables;
		variables = 0;
	}
	if( values != 0 ) {
		delete[] values;
		values = 0;
	}
	capacity = 0;
}

void CVariables::Move(CVariables* variablesTo)
{
	variablesTo->Reset();
	variablesTo->variables = variables;
	variablesTo->capacity = capacity;
	variablesTo->values = values;
	variables = 0;
	capacity = 0;
	values = 0;
}

void CVariables::Set(TVariableIndex variable, int value)
{
	allocValues();
	
	CVariable& var = variables[variable];
	values[var.currentOffset] = value;
	var.currentOffset++;
}

int CVariables::Get(TVariableIndex variable, TVariableType* type, bool* doMove)
{
	CVariable& var = variables[variable];
	*type = var.type;
	*doMove = false;
	if( var.currentOffset > var.initialOffset ) {
		*doMove = true;
		var.currentOffset--;
	}
	return values[var.currentOffset];
}

void CVariables::allocVariables(int countOfVariables)
{
	Reset();
	variables = new CVariable[countOfVariables];
}

void CVariables::allocValues()
{
	if( values == 0 ) {
		assert( capacity > 0 );
		values = new int[capacity];
	}
}

CVariablesBuilder::CVariablesBuilder(IVariablesBuilderListener* listener):
	CListener(listener)
{
	Reset();
}

void CVariablesBuilder::Reset()
{
	countOfVariables = 0;
	
	for( int i = 0; i < variablesSize; ++i ) {
		variables[i].type = '\0';
		variables[i].qualifier.Empty();
	}
}

TVariableIndex CVariablesBuilder::AddLeft(TVariableName name,
	TVariableType type, CQualifier* qualifier)
{
	assert( name >= 0 && name < variablesSize );
	
	CVariableInfo& var = variables[name];
	
	if( var.type == '\0' ) {
		if( type == 's' || type == 'w' || type == 'v' || type == 'e' ) {
			var.name = countOfVariables;
			var.type = type;
			var.count = 1;
			if( qualifier != 0 ) {
				qualifier->Move( &var.qualifier );
			}
			++countOfVariables;
			return var.name;
		} else {
			error( VBEC_NoSuchTypeOfVariable );
		}
	} else if( var.type == type ) {
		var.count++;
		if( qualifier != 0 ) {
			var.qualifier.DestructiveIntersection( qualifier );
		}
		return var.name;
	} else {
		error( VBEC_TypeOfVariableDoesNotMatch );
	}
	
	return InvalidVariableIndex;
}

TVariableIndex CVariablesBuilder::AddRight(TVariableName name, TVariableType type)
{
	assert( name >= 0 && name < variablesSize );

	CVariableInfo& var = variables[name];
	
	if( var.type == type ) {
		return var.name;
	}
	
	error( ( var.type == '\0' ) ? VBEC_NoSuchVariableInLeftPart :
		VBEC_TypeOfVariableDoesNotMatch );
	return InvalidVariableIndex;
}

void CVariablesBuilder::Export(CVariables* variablesObject)
{
	if( countOfVariables > 0 ) {
		variablesObject->allocVariables(countOfVariables);
		CVariables::CVariable* _variables = variablesObject->variables;
	
		int offset = 0;
		for( int i = 0; i < variablesSize; i++ ) {
			CVariableInfo& var = variables[i];
		
			if( var.type != '\0' ) {
				_variables[var.name].type = var.type;
				var.qualifier.Move( &_variables[var.name].qualifier );
				_variables[var.name].initialOffset = offset;
				_variables[var.name].currentOffset = offset;
				offset += var.count;
			}
		}
		variablesObject->setCapacity(offset);
		Reset();
	}
}

} // end of namespace refal2
