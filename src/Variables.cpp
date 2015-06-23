#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

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

void CVariables::Swap( CVariables& swapWith )
{
	std::swap( swapWith.variables, variables );
	std::swap( swapWith.variablesSize, variablesSize );
	std::swap( swapWith.variablesValues, variablesValues );
	std::swap( swapWith.variablesValuesSize, variablesValuesSize );
}

bool CVariables::IsFull( TVariableIndex variableIndex ) const
{
	assert( IsValidVariableIndex( variableIndex ) );
	CVariable& variable = variables[variableIndex];
	return ( variable.position == variable.topPosition );
}

bool CVariables::IsSet( TVariableIndex variableIndex ) const
{
	assert( IsValidVariableIndex( variableIndex ) );
	CVariable& variable = variables[variableIndex];
	return ( variable.position > variable.originPosition );
}

void CVariables::Set( TVariableIndex variableIndex, TTableIndex tableIndex )
{
	assert( IsValidVariableIndex( variableIndex ) );
	CVariable& variable = variables[variableIndex];
	if( variable.position < variable.topPosition ) {
		allocVariablesValues();
		variablesValues[variable.position] = tableIndex;
		variable.position++;
	}
}

TTableIndex CVariables::GetMainValue( TVariableIndex variableIndex ) const
{
	assert( IsSet( variableIndex ) );
	return variablesValues[variables[variableIndex].originPosition];
}

bool CVariables::Get( TVariableIndex variableIndex, TTableIndex& tableIndex)
{
	assert( IsValidVariableIndex( variableIndex ) );
	assert( variablesValues != 0 );
	CVariable& variable = variables[variableIndex];
	if( variable.position > variable.originPosition ) {
		variable.position--;
		tableIndex = variablesValues[variable.position];
		return true;
	} else /* var.position == var.originPosition */ {
		tableIndex = variablesValues[variable.position];
		return false;
	}
}

//-----------------------------------------------------------------------------

CVariablesBuilder::CVariablesBuilder( IErrorHandler* errorProcessor ):
	CErrorsHelper( errorProcessor )
{
	Reset();
}

void CVariablesBuilder::Reset()
{
	countOfVariables = 0;
	for( int i = 0; i < VariablesInfoSize; ++i ) {
		variables[i].type = InvalidVariableType;
		variables[i].qualifier.Empty();
	}
}

void CVariablesBuilder::Export( CVariables& v )
{
	v.Reset();
	if( countOfVariables > 0 ) {
		v.variablesSize = countOfVariables;
		v.variables = static_cast<CVariable*>(
			::operator new( v.variablesSize * sizeof( CVariable ) ) );
		v.variablesValuesSize = 0;
		for( int i = 0; i < VariablesInfoSize; i++ ) {
			CVariableInfo& vi = variables[i];
			if( vi.type != InvalidVariableType ) {
				int countOfValues = std::min( vi.countLeft, vi.countRight );
				new( v.variables + vi.name )CVariable( vi.type,
					v.variablesValuesSize, countOfValues );
				v.variablesValuesSize += countOfValues;
				vi.qualifier.Move( v.variables[vi.name].qualifier );
			}
		}
	}
	Reset();
}

TVariableIndex CVariablesBuilder::AddLeft( TVariableName name,
	TVariableType type, CQualifier* qualifier )
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
				qualifier->Move( var.qualifier );
			}
			++countOfVariables;
			return var.name;
		}
	} else if( var.type == type ) {
		var.countLeft++;
		if( qualifier != 0 ) {
			var.qualifier.DestructiveIntersection( *qualifier );
		}
		return var.name;
	} else {
		error( EC_TypeOfVariableDoesNotMatch );
	}
	return InvalidVariableIndex;
}

TVariableIndex CVariablesBuilder::AddRight( TVariableName name,
	TVariableType type,	CQualifier* qualifier )
{
	if( !checkName( name ) ) {
		return InvalidVariableIndex;
	}
	CVariableInfo& var = variables[name];
	if( var.type == type ) {
		var.countRight++;
		if( qualifier != 0 ) {
			var.qualifier.DestructiveIntersection( *qualifier );
		}
		return var.name;
	} else {
		if( var.type == InvalidVariableType ) {
			error( EC_NoSuchVariableInLeftPart );
		} else {
			error( EC_TypeOfVariableDoesNotMatch );
		}
		return InvalidVariableIndex;
	}
}


bool CVariablesBuilder::checkName( TVariableName name )
{
	if( CVariable::IsValidName( name ) ) {
		return true;
	} else {
		error( EC_InvalidVariableName );
		return false;
	}
}

bool CVariablesBuilder::checkType( TVariableType type )
{
	if( CVariable::IsValidType( type ) ) {
		return true;
	} else {
		error( EC_NoSuchTypeOfVariable );
		return false;
	}
}


void CVariablesBuilder::error( TErrorCode errorCode )
{
	switch( errorCode ) {
		case EC_InvalidVariableName:
			CErrorsHelper::Error( "invalid variable name" );
			break;
		case EC_NoSuchTypeOfVariable:
			CErrorsHelper::Error( "no such type of variable" );
			break;
		case EC_TypeOfVariableDoesNotMatch:
			CErrorsHelper::Error( "type of variable does not match" );
			break;
		case EC_NoSuchVariableInLeftPart:
			CErrorsHelper::Error( "no such variable in left part" );
			break;
		default:
			assert( false );
			break;
	}
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
