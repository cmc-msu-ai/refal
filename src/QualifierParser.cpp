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

void CQualifierParser::BeginQualifer()
{
	resetParser();
}

void CQualifierParser::BeginNamedQualifier( const std::string& _name )
{
	assert( !_name.empty() );
	resetParser();
	name = _name;
	MakeLower( name );
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
	name.empty();
	afterRightParen = false;
	builder.Reset();
}

void CQualifierParser::addNamedQualifier( const CToken& token )
{
	typedef std::pair<CNamedQualifiers::iterator, bool> CPair;
	CPair pair = namedQualifiers.insert( std::make_pair( name, CQualifier() ) );
	if( !pair.second ) {
		error( token, "qualifier `" + name + "` already defined." );
	} else {
		builder.Export( pair.first->second );
	}
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
	const std::string& word = token.value.word;
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
	builder.AddLabel( LabelTable.AddLabel( token.value.word ) );
	afterRightParen = false;
}

void CQualifierParser::addNumber( const CToken& token )
{
	builder.AddNumber( token.value.number );
	afterRightParen = false;
}

void CQualifierParser::addString( const CToken& token )
{
	const std::vector<char>& string = token.value.string;
	for( std::vector<char>::const_iterator i = string.begin();
		i != string.end(); ++i )
	{
		builder.AddChar( *i );
	}
	afterRightParen = false;
}

void CQualifierParser::addQualifier( const CToken& token )
{
	error( token, "not implemented yet" );
	afterRightParen = false;
}

void CQualifierParser::addLineFeed( const CToken& token )
{
	if( name.empty() ) {
		error( token, "unexpected line feed in variable qualifier" );
	} else if( builder.IsNegative() ) {
		error( token, "no parenthesis balance in qualifier" );
	} else {
		if( !afterRightParen ) {
			builder.IsNegative();
		}
		addNamedQualifier( token );
		parsed = true;
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
	} else if( name.empty() ) {
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
