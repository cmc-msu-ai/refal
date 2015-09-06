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

CVariablesBuilder::CVariablesBuilder( IErrorHandler* errorHandler ):
	CErrorsHelper( errorHandler ),
	firstFreeVariableIndex( 0 )
{
	Reset();
}

void CVariablesBuilder::Reset()
{
	for( int i = 0; i < firstFreeVariableIndex; i++ ) {
		variables[i].Qualifier.Empty();
	}
	firstFreeVariableIndex = 0;
	for( int i = 0; i < VariableNameToIndexSize; i++ ) {
		variableNameToIndex[i] = InvalidVariableIndex;
	}
}

void CVariablesBuilder::Export( CVariables& v )
{
	v.Reset();
	if( firstFreeVariableIndex > 0 ) {
		v.variablesSize = firstFreeVariableIndex;
		v.variables = static_cast<CVariable*>(
			::operator new( v.variablesSize * sizeof( CVariable ) ) );
		v.variablesValuesSize = 0;
		for( int i = 0; i < firstFreeVariableIndex; i++ ) {
			CVariableData& vd = variables[i];
			assert( vd.Type != VT_None );
			const int countOfValues = std::min( vd.CountLeft, vd.CountRight );
			new( v.variables + i )CVariable( vd.Type,
				v.variablesValuesSize, countOfValues );
			v.variablesValuesSize += countOfValues;
			vd.Qualifier.Move( v.variables[i].qualifier );
		}
	}
	Reset();
}

TVariableIndex CVariablesBuilder::AddLeft( const TVariableName name,
	const TVariableTypeTag type )
{
	const CQualifier* defaultQualifier = nullptr;
	const TVariableType variableType = checkTypeTag( type, &defaultQualifier );
	if( variableType != VT_None && checkName( name ) ) {
		if( variableNameToIndex[name] == InvalidVariableIndex ) {
			variableNameToIndex[name] = firstFreeVariableIndex;
			firstFreeVariableIndex++;
			const TVariableIndex variableIndex = variableNameToIndex[name];
			CVariableData& variable = variables[variableIndex];
			variable.Type = variableType;
			variable.CountLeft = 1;
			variable.CountRight = 0;
			assert( variable.Qualifier.IsEmpty() );
			if( defaultQualifier != nullptr ) {
				variable.Qualifier = *defaultQualifier;
			}
			return variableIndex;
		} else {
			const TVariableIndex variableIndex = variableNameToIndex[name];
			CVariableData& variable = variables[variableIndex];
			if( variable.Type == variableType ) {
				variable.CountRight++;
				return variableIndex;
			} else {
				error( EC_TypeOfVariableDoesNotMatch );
			}
		}
	}
	return InvalidVariableIndex;
}

TVariableIndex CVariablesBuilder::AddRight( const TVariableName name,
	const TVariableTypeTag type )
{
	const TVariableType variableType = checkTypeTag( type );
	if( variableType != VT_None && checkName( name ) ) {
		const TVariableIndex variableIndex = variableNameToIndex[name];
		if( variableIndex != InvalidVariableIndex ) {
			CVariableData& variable = variables[variableIndex];
			if( variable.Type == variableType ) {
				variable.CountRight++;
				return variableIndex;
			} else {
				error( EC_TypeOfVariableDoesNotMatch );
			}
		} else {
			error( EC_NoSuchVariableInLeftPart );
		}
	}
	return InvalidVariableIndex;
}

void CVariablesBuilder::AddQualifier( const TVariableIndex variable,
	CQualifier& qualifier )
{
	assert( variable >= 0 && variable < firstFreeVariableIndex );
	CVariableData& variableData = variables[variable];
	variableData.Qualifier.DestructiveIntersection( qualifier );
}

bool CVariablesBuilder::checkName( const TVariableName name )
{
	if( ::isalnum( name ) != 0 || name == '_' ) {
		return true;
	}
	error( EC_InvalidVariableName );
	return false;
}

static const CQualifier* getDefaultQualifierForN()
{
	static CQualifier* const qualifier = new CQualifier;
	if( qualifier == 0 ) {
		CQualifierBuilder qualifierBuilder;
		qualifierBuilder.AddN();
		qualifierBuilder.AddNegative();
		qualifierBuilder.Export( *qualifier );
	}
	return qualifier;
}

static const CQualifier* getDefaultQualifierForF()
{
	static CQualifier* const qualifier = new CQualifier;
	if( qualifier == 0 ) {
		CQualifierBuilder qualifierBuilder;
		qualifierBuilder.AddF();
		qualifierBuilder.AddNegative();
		qualifierBuilder.Export( *qualifier );
	}
	return qualifier;
}

TVariableType CVariablesBuilder::checkTypeTag( const TVariableTypeTag type,
	const CQualifier** defaultQualifierForTag )
{
	switch( type ) {
		case 's':
		case 'S':
			return VT_S;
		case 'w':
		case 'W':
			return VT_W;
		case 'v':
		case 'V':
			return VT_V;
		case 'e':
		case 'E':
			return VT_E;
		case 'n':
		case 'N':
			if( defaultQualifierForTag != nullptr ) {
				*defaultQualifierForTag = getDefaultQualifierForN();
			}
			// todo: warning
			return VT_S;
		case 'f':
		case 'F':
			if( defaultQualifierForTag != nullptr ) {
				*defaultQualifierForTag = getDefaultQualifierForF();
			}
			// todo: warning
			return VT_S;
	}
	error( EC_NoSuchTypeOfVariable );
	return VT_None;
}

void CVariablesBuilder::error( TErrorCode errorCode )
{
	CError::SetSeverity( ES_Error );
	switch( errorCode ) {
		case EC_InvalidVariableName:
			CError::SetMessage( "invalid variable name" );
			break;
		case EC_NoSuchTypeOfVariable:
			CError::SetMessage( "no such type of variable" );
			break;
		case EC_TypeOfVariableDoesNotMatch:
			CError::SetMessage( "type of variable does not match" );
			break;
		case EC_NoSuchVariableInLeftPart:
			CError::SetMessage( "no such variable in left part" );
			break;
		default:
			assert( false );
			break;
	}
	CErrorsHelper::Error();
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
