#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CQualifierParser::CQualifierParser( IErrorHandler* errorHandler ):
	CModuleBuilder( errorHandler )
{
	Reset();
}

void CQualifierParser::Reset()
{
	CModuleBuilder::Reset();
	resetParser();
}

void CQualifierParser::StartQualifer()
{
	resetParser();
}

void CQualifierParser::StartNamedQualifier()
{
	assert( token.type == TT_Word );
	assert( !token.word.empty() );
	resetParser();
	namedQualifier = token;
}

void CQualifierParser::AddToken()
{
	assert( !IsFinished() );
	switch( token.type ) {
		case TT_Blank:
			break;
		case TT_Word:
			addWord();
			break;
		case TT_Label:
			addLabel();
			break;
		case TT_Number:
			addNumber();
			break;
		case TT_String:
			addString();
			break;
		case TT_LineFeed:
			addLineFeed();
			break;
		case TT_Qualifier:
			addQualifier();
			break;
		case TT_LeftParen:
			addLeftParen();
			break;
		case TT_RightParen:
			addRightParen();
			break;
		case TT_Comma:
		case TT_Equal:
		case TT_LeftBracket:
		case TT_RightBracket:
			error( "unexpected token in qualifier" );
			break;
		default:
			assert( false );
			break;
	}
}

void CQualifierParser::GetQualifier( CQualifier& qualifier )
{
	assert( IsCorrect() );
	builder.Export( qualifier );
}

void CQualifierParser::resetParser()
{
	CParsingElementState::Reset();
	afterRightParen = false;
	builder.Reset();
	namedQualifier = CToken();
}

void CQualifierParser::error( const std::string& message )
{
	SetWrong();
	CErrorsHelper::RaiseError( ES_Error, message, token );
}

void CQualifierParser::addWord()
{
	for( std::string::size_type i = 0; i < token.word.length(); i++ ) {
		switch( token.word[i] ) {
			case 's': case 'S':
				builder.AddS();
				break;
			case 'f': case 'F':
				builder.AddF();
				break;
			case 'n': case 'N':
				builder.AddN();
				break;
			case 'o': case 'O':
				builder.AddO();
				break;
			case 'l': case 'L':
				builder.AddL();
				break;
			case 'd': case 'D':
				builder.AddD();
				break;
			case 'w': case 'W':
				builder.AddW();
				break;
			case 'b': case 'B':
				builder.AddB();
				break;
			default:
				token.position += i;
				error( "primary qualifier `" +
					std::string( 1, token.word[i] ) + "` does not exist" );
				return;
		}
	}
	afterRightParen = false;
}

void CQualifierParser::addLabel()
{
	builder.AddLabel( CModuleBuilder::Declare( token ) );
	afterRightParen = false;
}

void CQualifierParser::addNumber()
{
	builder.AddNumber( token.number );
	afterRightParen = false;
}

void CQualifierParser::addString()
{
	for( std::string::size_type i = 0; i < token.word.length(); i++ ) {
		builder.AddChar( token.word[i] );
	}
	afterRightParen = false;
}

void CQualifierParser::addQualifier()
{
	CQualifier qualifier;
	if( CModuleBuilder::GetNamedQualifier( token, qualifier ) ) {
		builder.AddQualifier( qualifier );
		afterRightParen = false;
	} else {
		SetWrong();
	}
}

void CQualifierParser::addLineFeed()
{
	if( namedQualifier.IsNone() ) {
		error( "unexpected line feed in variable qualifier" );
	} else if( builder.IsNegative() ) {
		error( "no parenthesis balance in qualifier" );
	} else {
		if( !afterRightParen ) {
			builder.AddNegative();
		}
		SetCorrect();
		CQualifier qualifier;
		GetQualifier( qualifier );
		if( !CModuleBuilder::SetNamedQualifier( namedQualifier, qualifier ) ) {
			SetWrong();
		}
	}
}

void CQualifierParser::addLeftParen()
{
	if( builder.IsNegative() ) {
		error( "unexpected left parenthesis in qualifier" );
	} else {
		builder.AddNegative();
		afterRightParen = false;
	}
}

void CQualifierParser::addRightParen()
{
	if( builder.IsNegative() ) {
		builder.AddNegative();
		afterRightParen = true;
	} else if( namedQualifier.IsNone() ) {
		if( !afterRightParen ) {
			builder.AddNegative();
		}
		SetCorrect();
	} else {
		error( "unexpected right parenthesis in named qualifier" );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
