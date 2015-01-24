#include <Refal2.h>

namespace Refal2 {
	
void CVariables::Reset()
{
	if( variables != 0 ) {
		::operator delete( variables );
		variables = 0;
	}
	variablesSize = 0;

	if( variablesValues != 0 ) {
		::operator delete( variablesValues );
		variablesValues = 0;
	}
	variablesValuesSize = 0;
}

void CVariables::Import(CVariablesBuilder* variablesBuiler)
{
	Reset();

	if( variablesBuiler->countOfVariables > 0 ) {
		variablesSize = variablesBuiler->countOfVariables;
		variables = static_cast<CVariable*>(
			::operator new( variablesSize * sizeof(CVariable) ) );

		variablesValuesSize = 0;
		for( int i = 0, j = 0; i < CVariablesBuilder::variablesInfoSize; i++ ) {
			CVariablesBuilder::CVariableInfo& variableInfo =
				variablesBuiler->variables[i];

			if( variableInfo.type != InvalidVariableType ) {
				int countOfValues =
					std::min( variableInfo.countLeft, variableInfo.countRight );
				new( variables + j )CVariable( variableInfo.type,
					variablesValuesSize, countOfValues );
				variablesValuesSize += countOfValues;

				variableInfo.qualifier.Move( &variables[j].qualifier );
				j++;
			}
		}
	}

	variablesBuiler->Reset();
}

void CVariables::Swap(CVariables* swapWith)
{
	std::swap( swapWith->variables, variables );
	std::swap( swapWith->variablesSize, variablesSize );
	std::swap( swapWith->variablesValues, variablesValues );
	std::swap( swapWith->variablesValuesSize, variablesValuesSize );
}

bool CVariables::IsFull(const TVariableIndex variableIndex) const
{
	assert( IsValidVariableIndex( variableIndex ) );

	CVariable& variable = variables[variableIndex];

	return ( variable.position == variable.topPosition );
}

bool CVariables::IsSet(const TVariableIndex variableIndex) const
{
	assert( IsValidVariableIndex( variableIndex ) );

	CVariable& variable = variables[variableIndex];

	return ( variable.position > variable.originPosition );
}

void CVariables::Set(const TVariableIndex variableIndex,
	const TTableIndex tableIndex)
{
	assert( IsValidVariableIndex( variableIndex ) );

	CVariable& variable = variables[variableIndex];

	if( variable.position < variable.topPosition ) {
		allocVariablesValues();

		variablesValues[variable.position] = tableIndex;
		variable.position++;
	}
}

bool CVariables::Get(const TVariableIndex variableIndex,
	TTableIndex* tableIndex)
{
	assert( IsValidVariableIndex( variableIndex ) );
	assert( variablesValues != 0 );

	CVariable& variable = variables[variableIndex];

	if( variable.position > variable.originPosition ) {
		variable.position--;
		*tableIndex = variablesValues[variable.position];
		return true;
	} else /* var.position == var.originPosition */ {
		*tableIndex = variablesValues[variable.position];
		return false;
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
	
	for( int i = 0; i < variablesInfoSize; ++i ) {
		variables[i].type = InvalidVariableType;
		variables[i].qualifier.Empty();
	}
}

TVariableIndex CVariablesBuilder::AddLeft(TVariableName name,
	TVariableType type, CQualifier* qualifier)
{
	if( !checkName( name ) ) {
		return InvalidVariableIndex;
	}

	CVariableInfo& var = variables[name];

	if( var.type == InvalidVariableType ) {
		if( checkType( type ) ) {
			var.name = countOfVariables;
			var.type = type;
			var.countLeft = 1;
			var.countRight = 0;
			if( qualifier != 0 ) {
				qualifier->Move( &var.qualifier );
			}
			++countOfVariables;
			return var.name;
		}
	} else if( var.type == type ) {
		var.countLeft++;
		if( qualifier != 0 ) {
			var.qualifier.DestructiveIntersection( qualifier );
		}
		return var.name;
	} else {
		error( VBEC_TypeOfVariableDoesNotMatch );
	}
	
	return InvalidVariableIndex;
}

TVariableIndex CVariablesBuilder::AddRight(TVariableName name,
	TVariableType type)
{
	if( !checkName( name ) ) {
		return InvalidVariableIndex;
	}

	CVariableInfo& var = variables[name];
	
	if( var.type == type ) {
		var.countRight++;
		return var.name;
	} else {
		error( ( var.type == InvalidVariableType ) ?
			VBEC_NoSuchVariableInLeftPart : VBEC_TypeOfVariableDoesNotMatch );
		return InvalidVariableIndex;
	}
}

} // end of namespace refal2
