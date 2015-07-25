#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

bool CUnit::IsEqualWith( const CUnit& unit ) const
{
	if( type != unit.type ) {
		return false;
	}
	switch( type ) {
		case UT_Char:
			return ( c == unit.c );
			break;
		case UT_Label:
			return ( label == unit.label );
			break;
		case UT_Number:
			return ( number == unit.number );
			break;
		case UT_Variable:
			return ( variable == unit.variable );
			break;
		case UT_LeftParen:
		case UT_RightParen:
		case UT_LeftBracket:
		case UT_RightBracket:
			return true;
		default:
			break;
	}
	assert( false );
	return false;
}

void CUnit::Print( std::ostream& outputStream,
	const CVariables* variables ) const
{
	switch( GetType() ) {
		case UT_Char:
			outputStream << "'" << Char() << "' ";
			break;
		case UT_Label:
			outputStream << "/L:" << Label() << "/ ";
			break;
		case UT_Number:
			outputStream << "/" << Number() << "/ ";
			break;
		case UT_Variable:
			if( variables != 0 ) {
				outputStream << variables->GetVariable( Variable() ).GetType();
				variables->GetVariable( Variable() ).GetQualifier().Print(
					outputStream );
			} else {
				outputStream << "V";
			}
			outputStream << Variable() << " ";
			break;
		case UT_LeftParen:
			outputStream << "( ";
			break;
		case UT_RightParen:
			outputStream << ") ";
			break;
		case UT_LeftBracket:
			outputStream << "< ";
			break;
		case UT_RightBracket:
			outputStream << "> ";
			break;
		default:
			assert( false );
			break;
	}
}

//-----------------------------------------------------------------------------

void CUnitList::Print( std::ostream& outputStream,
	const CVariables* variables ) const
{
	for( const CUnitNode* node = GetFirst(); node != 0; node = node->Next() ) {
		node->Print( outputStream, variables );
	}
}

void CUnitList::HandyPrint( std::ostream& outputStream ) const
{
	bool lastWasChar = false;
	for( const CUnitNode* node = GetFirst(); node != 0; node = node->Next() ) {
		if( node->GetType() == UT_Char ) {
			if( !lastWasChar ) {
				outputStream << "'";
				lastWasChar = true;
			}
		} else {
			if( lastWasChar ) {
				outputStream << "'";
				lastWasChar = false;
			}
		}
		switch( node->GetType() ) {
			case UT_Char:
				outputStream << node->Char();
				break;
			case UT_Label:
				outputStream << "/L:" << node->Label() << "/";
				break;
			case UT_Number:
				outputStream << "/" << node->Number() << "/";
				break;
			case UT_LeftParen:
				outputStream << "(";
				break;
			case UT_RightParen:
				outputStream << ")";
				break;
			case UT_LeftBracket:
				outputStream << "<";
				break;
			case UT_RightBracket:
				outputStream << ">";
				break;
			default:
				assert( false );
				break;
		}
	}
	if( lastWasChar ) {
		outputStream << "'";
	}
	outputStream << std::endl;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
