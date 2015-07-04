#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

CQualifierParser::CQualifierParser( IErrorHandler* errorHandler ):
	CErrorsHelper( errorHandler )
{
	Reset();
}

void CQualifierParser::Reset()
{
	resetParser();
	namedQualifiers.clear();
}

void CQualifierParser::StartQualifer()
{
	resetParser();
}

bool CQualifierParser::StartNamedQualifier( CToken& nameToken )
{
	assert( nameToken.type == TT_Word );
	assert( !nameToken.value.text.empty() );
	resetParser();
	std::string& name = nameToken.value.text;
	MakeLower( name );
	typedef std::pair<CNamedQualifiers::iterator, bool> CPair;
	CPair pair = namedQualifiers.insert( std::make_pair( name, CQualifier() ) );
	if( !pair.second ) {
		error( nameToken, "qualifier `" + name + "` already defined" );
		return false;
	}
	currentNamedQualifier = pair.first;
	return true;
}

bool CQualifierParser::AddToken( CToken& token )
{
	assert( !parsed );
	switch( token.type ) {
		case TT_Word:
			addWord( token );
			break;
		case TT_Label:
			addLabel( token );
			break;
		case TT_Number:
			addNumber( token );
			break;
		case TT_String:
			addString( token );
			break;
		case TT_LineFeed:
			addLineFeed( token );
			break;
		case TT_Qualifier:
			addQualifier( token );
			break;
		case TT_LeftParen:
			addLeftParen( token );
			break;
		case TT_RightParen:
			addRightParen( token );
			break;
		case TT_Blank:
		case TT_Comma:
		case TT_Equal:
		case TT_LeftBracket:
		case TT_RightBracket:
			error( token, "unexpected token in qualifier" );
			break;
		default:
			assert( false );
			break;
	}
	return parsed;
}

void CQualifierParser::GetQualifier( CQualifier& qualifier )
{
	assert( parsed && !HasErrors() );
	builder.Export( qualifier );
}

void CQualifierParser::resetParser()
{
	CErrorsHelper::Reset();
	parsed = false;
	afterRightParen = false;
	builder.Reset();
	currentNamedQualifier = namedQualifiers.end();
}

void CQualifierParser::error( const CToken& token, const std::string& message )
{
	parsed = true;
	std::ostringstream errorStream;
	errorStream << ":" << token.line << ":" << token.position
		<< ": error in qualifier: " << message << ".";
	CErrorsHelper::Error( errorStream.str() );
}

void CQualifierParser::addWord( CToken& token )
{
	const std::string& word = token.value.text;
	for( std::string::size_type i = 0; i < word.length(); i++ ) {
		switch( word[i] ) {
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
				error( token, "primary qualifier `" + std::string( 1, word[i] )
					+ "` does not exist" );
				return;
		}
	}
	afterRightParen = false;
}

void CQualifierParser::addLabel( const CToken& token )
{
	builder.AddLabel( LabelTable.AddLabel( token.value.text ) );
	afterRightParen = false;
}

void CQualifierParser::addNumber( const CToken& token )
{
	builder.AddNumber( token.value.number );
	afterRightParen = false;
}

void CQualifierParser::addString( const CToken& token )
{
	const std::string& text = token.value.text;
	for( std::string::size_type i = 0; i < text.length(); i++ ) {
		builder.AddChar( text[i] );
	}
	afterRightParen = false;
}

void CQualifierParser::addQualifier( CToken& token )
{
	assert( !token.value.text.empty() );
	std::string& name = token.value.text;
	MakeLower( name );
	CNamedQualifiers::iterator namedQualifier = namedQualifiers.find( name );
	if( namedQualifier == namedQualifiers.end() ) {
		error( token, "qualifier `" + name + "` wasn't defined" );
	} else {
		builder.AddQualifier( namedQualifier->second );
		afterRightParen = false;
	}
}

void CQualifierParser::addLineFeed( const CToken& token )
{
	if( currentNamedQualifier == namedQualifiers.end() ) {
		error( token, "unexpected line feed in variable qualifier" );
	} else if( builder.IsNegative() ) {
		error( token, "no parenthesis balance in qualifier" );
	} else {
		if( !afterRightParen ) {
			builder.IsNegative();
		}
		parsed = true;
		GetQualifier( currentNamedQualifier->second );
	}
}

void CQualifierParser::addLeftParen( const CToken& token )
{
	if( builder.IsNegative() ) {
		error( token, "unexpected left paren in qualifier" );
	} else {
		builder.AddNegative();
		afterRightParen = false;
	}
}

void CQualifierParser::addRightParen( const CToken& token )
{
	if( builder.IsNegative() ) {
		builder.AddNegative();
		afterRightParen = true;
	} else if( currentNamedQualifier == namedQualifiers.end() ) {
		if( !afterRightParen ) {
			builder.IsNegative();
		}
		parsed = true;
	} else {
		error( token, "unexpected right paren in named qualifier" );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
