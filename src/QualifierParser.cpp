#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CQualifierParser::CQualifierParser( IErrorHandler* errorHandler ):
	CFunctionBuilder( errorHandler )
{
	Reset();
}

void CQualifierParser::Reset()
{
	CFunctionBuilder::Reset();
	namedQualifiers.clear();
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
	std::string& name = token.word;
	MakeLower( name );
	typedef std::pair<CNamedQualifiers::iterator, bool> CPair;
	CPair pair = namedQualifiers.insert( std::make_pair( name, CQualifier() ) );
	if( pair.second ) {
		currentNamedQualifier = pair.first;
	} else {
		error( "qualifier `" + name + "` already defined" );
	}
}

void CQualifierParser::AddToken()
{
	assert( !IsFinished() );
	switch( token.type ) {
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
		case TT_Blank:
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

void CQualifierParser::GetNamedQualifier( CQualifier& qualifier )
{
	assert( token.type == TT_Qualifier );
	assert( !token.word.empty() );
	MakeLower( token.word );
	CNamedQualifiers::iterator i = namedQualifiers.find( token.word );
	if( i == namedQualifiers.end() ) {
		error( "qualifier `" + token.word + "` wasn't defined" );
	} else {
		qualifier = i->second;
	}
}

void CQualifierParser::resetParser()
{
	CParsingElementState::Reset();
	afterRightParen = false;
	builder.Reset();
	currentNamedQualifier = namedQualifiers.end();
}

void CQualifierParser::error( const std::string& message )
{
	SetWrong();
	CErrorsHelper::Error( message );
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
	MakeLower( token.word );
	builder.AddLabel( labels.AddLabel( token.word ) );
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
	GetNamedQualifier( qualifier );
	if( !IsWrong() ) {
		builder.AddQualifier( qualifier );
		afterRightParen = false;
	}
}

void CQualifierParser::addLineFeed()
{
	if( currentNamedQualifier == namedQualifiers.end() ) {
		error( "unexpected line feed in variable qualifier" );
	} else if( builder.IsNegative() ) {
		error( "no parenthesis balance in qualifier" );
	} else {
		if( !afterRightParen ) {
			builder.IsNegative();
		}
		SetCorrect();
		GetQualifier( currentNamedQualifier->second );
	}
}

void CQualifierParser::addLeftParen()
{
	if( builder.IsNegative() ) {
		error( "unexpected left paren in qualifier" );
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
	} else if( currentNamedQualifier == namedQualifiers.end() ) {
		if( !afterRightParen ) {
			builder.IsNegative();
		}
		SetCorrect();
	} else {
		error( "unexpected right paren in named qualifier" );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
