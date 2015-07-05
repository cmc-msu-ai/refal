#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CRuleParser::CRuleParser( IErrorHandler* errorHandler ):
	CQualifierParser( errorHandler )
{
	Reset();
}

void CRuleParser::Reset()
{
	CQualifierParser::Reset();
	SetWrong();
}

void CRuleParser::BeginFunction()
{
	BeginRule();
}

void CRuleParser::EndFunction()
{
	//assert( IsFinished() );
}

void CRuleParser::BeginRule()
{
	//assert( IsFinished() );
	CParsingElementState::Reset();
	state = S_Direction;
}

void CRuleParser::AddToken()
{
	assert( !IsFinished() );
	switch( state ) {
		case S_Direction:
			direction();
			break;
		case S_AfterDirection:
			afterDirection();
			break;
		case S_AfterLeftBracket:
			afterLeftBracket();
			break;
		case S_AfterVariableType:
			afterVariableType();
			break;
		case S_VariableQualifier:
			variableQualifier();
			break;
		case S_AfterVariableQualifier:
			afterVariableQualifier();
			break;
		default:
			assert( false );
			break;
	}
}

void CRuleParser::error( const std::string& message )
{
	SetWrong();
	CErrorsHelper::Error( message );
}

void CRuleParser::direction()
{
	if( token.type == TT_Blank ) {
		return;
	}
	state = S_AfterDirection;
	if( token.type == TT_Word && token.word.length() == 1 ) {
		switch( token.word[0] ) {
			case 'r':
			case 'R':
				// TODO: set right direction
			case 'l':
			case 'L':
				return;
		}
	}
	AddToken();
}

void CRuleParser::afterDirection()
{
	switch( token.type ) {
		case TT_Blank:
			break;
		case TT_Equal:
			CFunctionBuilder::AddEndOfLeft();
			break;
		case TT_Comma:
			error( "unexpected token in the function rule" );
			break;
		case TT_Label:
			MakeLower( token.word );
			CFunctionBuilder::AddLabel( labels.AddLabel( token.word ) );
			break;
		case TT_Number:
			CFunctionBuilder::AddNumber( token.number );
			break;
		case TT_String:
			for( std::string::size_type i = 0; i < token.word.length(); i++ ) {
				CFunctionBuilder::AddChar( token.word[i] );
			}
			break;
		case TT_Qualifier:
			error( "unexpected qualifier in the function rule" );
			break;
		case TT_LeftParen:
			CFunctionBuilder::AddLeftParen();
			break;
		case TT_RightParen:
			CFunctionBuilder::AddRightParen();
			break;
		case TT_LeftBracket:
			state = S_AfterLeftBracket;
			CFunctionBuilder::AddLeftBracket();
			break;
		case TT_RightBracket:
			CFunctionBuilder::AddRightBracket();
			break;
		case TT_Word:
			wordAfterDirection();
			break;
		case TT_LineFeed:
			CFunctionBuilder::AddEndOfRight();
			SetCorrect();
			break;
		default:
			assert( false );
			break;
	}
}

void CRuleParser::wordAfterDirection()
{
	assert( token.type == TT_Word );
	while( !token.word.empty() ) {
		if( token.word[0] == 'k' ) {
			CFunctionBuilder::AddLeftBracket();
			token.position++;
			token.word.erase( 0, 1 );
		} else {
			variableType = token.word[0];
			if( token.word.length() > 1 ) {
				CFunctionBuilder::AddVariable( variableType, token.word[1] );
				token.position += 2;
				token.word.erase( 0, 2 );
			} else {
				state = S_AfterVariableType;
				return;
			}
		}
	}
}

void CRuleParser::afterLeftBracket()
{
	state = S_AfterDirection;
	if( token.type == TT_Word ) {
		MakeLower( token.word );
		CFunctionBuilder::AddLabel( labels.AddLabel( token.word ) );
	} else {
		AddToken();
	}
}

void CRuleParser::afterVariableType()
{
	if( token.type == TT_LeftParen ) {
		CQualifierParser::StartQualifer();
		state = S_VariableQualifier;
	} else if( token.type == TT_Qualifier ) {
		state = S_AfterVariableQualifier;
		CQualifierParser::GetNamedQualifier( qualifier );
	} else {
		error( "unexpected token after variable type" );
	}
}

void CRuleParser::variableQualifier()
{
	CQualifierParser::AddToken();
	if( IsCorrect() ) {
		CQualifierParser::GetQualifier( qualifier );
		CParsingElementState::Reset();
		state = S_AfterVariableQualifier;
	}
}

void CRuleParser::afterVariableQualifier()
{
	if( token.type == TT_Word ) {
		state = S_AfterDirection;
		CFunctionBuilder::AddVariable( variableType, token.word[0], &qualifier );
		token.word.erase( 0, 1 );
		token.position++;
		wordAfterDirection();
	} else {
		error( "missing variable name" );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
