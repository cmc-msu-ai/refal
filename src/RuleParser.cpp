#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const char* const RuleLeftMatchingDirectionTag = "l";
const char* const RuleRightMatchingDirectionTag = "r";
const char RuleLeftBracketTag = 'k';

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

bool CRuleParser::BeginFunction()
{
	assert( token.type == TT_Word && !token.word.empty() );
	if( CModuleBuilder::Declare( token ) != InvalidDictionaryIndex ) {
		functionName = token;
		beginRule();
		return true;
	}
	return false;
}

void CRuleParser::EndFunction()
{
	if( !functionName.IsNone() ) {
		assert( !functionName.word.empty() );
		CRulePtr firstRule;
		CFunctionBuilder::Export( firstRule );
		if( static_cast<bool>( firstRule ) ) {
			CModuleBuilder::SetOrdinary( functionName, firstRule );
		} else {
			CModuleBuilder::SetEmpty( functionName );
		}
		functionName = CToken();
	}
}

bool CRuleParser::BeginRule()
{
	if( !functionName.IsNone() ) {
		beginRule();
		return true;
	}
	CParsingElementState::Reset();
	error( "try to define rule out of function" );
	return false;
}

void CRuleParser::AddToken()
{
	assert( !IsFinished() );
	CError::SetToken( token );
	( this->*state )();
	CError::ResetToken();
}

void CRuleParser::beginRule()
{
	CParsingElementState::Reset();
	assert( !functionName.IsNone() );
	state = &CRuleParser::direction;
}

void CRuleParser::error( const std::string& message )
{
	SetWrong();
	CErrorsHelper::RaiseError( ES_Error, message, token );
}

void CRuleParser::direction()
{
	if( token.type == TT_Blank ) {
		return;
	}
	state = &CRuleParser::afterDirection;
	if( token.type == TT_Word ) {
		if( CompareNoCase( token.word, RuleRightMatchingDirectionTag ) ) {
			CFunctionBuilder::SetRightDirection();
			return;
		} else if( CompareNoCase( token.word, RuleLeftMatchingDirectionTag ) ) {
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
		if( CompareNoCase( token.word[0], RuleLeftBracketTag ) ) {
			CFunctionBuilder::AddLeftBracket();
			token.position++;
			token.word.erase( 0, 1 );
		} else {
			variableTypeTag = token.word[0];
			if( token.word.length() > 1 ) {
				CFunctionBuilder::AddVariable( variableTypeTag, token.word[1] );
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
		CFunctionBuilder::AddVariable( variableTypeTag, token.word[0],
			&qualifier );
		token.word.erase( 0, 1 );
		token.position++;
		wordAfterDirection();
	} else {
		error( "missing variable name" );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
