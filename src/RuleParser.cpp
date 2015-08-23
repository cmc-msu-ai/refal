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
	functionName = token;
	BeginRule();
}

void CRuleParser::EndFunction()
{
	//assert( IsFinished() );
	if( !functionName.word.empty() ) {
		CRulePtr firstRule;
		CFunctionBuilder::Export( firstRule );
		if( static_cast<bool>( firstRule ) ) {
			CModuleBuilder::SetOrdinary( functionName, firstRule );
		} else {
			CModuleBuilder::SetEmpty( functionName );
		}
		functionName.word.clear();
	}
}

void CRuleParser::BeginRule()
{
	//assert( IsFinished() );
	CParsingElementState::Reset();
	state = &CRuleParser::direction;
}

void CRuleParser::AddToken()
{
	assert( !IsFinished() );
	( this->*state )();
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
	state = &CRuleParser::afterDirection;
	if( token.type == TT_Word && token.word.length() == 1 ) {
		switch( token.word[0] ) {
			case 'r':
			case 'R':
				CFunctionBuilder::SetRightDirection();
				return;
			case 'l':
			case 'L':
				return;
			default:
				break;
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
			CFunctionBuilder::AddLabel( CModuleBuilder::Declare( token ) );
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
			state = &CRuleParser::afterLeftBracket;
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
				state = &CRuleParser::afterVariableType;
				return;
			}
		}
	}
}

void CRuleParser::afterLeftBracket()
{
	state = &CRuleParser::afterDirection;
	if( token.type == TT_Word ) {
		CFunctionBuilder::AddLabel( CModuleBuilder::Declare( token ) );
	} else {
		AddToken();
	}
}

void CRuleParser::afterVariableType()
{
	if( token.type == TT_LeftParen ) {
		CQualifierParser::StartQualifer();
		state = &CRuleParser::variableQualifier;
	} else if( token.type == TT_Qualifier ) {
		if( CModuleBuilder::GetNamedQualifier( token, qualifier ) ) {
			state = &CRuleParser::afterVariableQualifier;
		} else {
			SetWrong();
		}
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
		state = &CRuleParser::afterVariableQualifier;
	}
}

void CRuleParser::afterVariableQualifier()
{
	if( token.type == TT_Word ) {
		state = &CRuleParser::afterDirection;
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
