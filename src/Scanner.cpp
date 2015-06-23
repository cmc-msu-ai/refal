#include <Refal2.h>
#include <sstream>
#include <iostream>

namespace Refal2 {

//-----------------------------------------------------------------------------

const char Dot = '.';
const char Plus = '+';
const char Zero = '0';
const char Blank = ' ';
const char Comma = ',';
const char Equal = '=';
const char Quote = '\'';
const char Hyphen = '-';
const char LineFeed = '\n';
const char NullByte = '\0';
const char Backslash = '\\';
const char Underline = '_';
const char LeftParen = '(';
const char RightParen = ')';
const char LeftBracket = '<';
const char RightBracket = '>';
const char CarriageReturn = '\r';
const char LimiterOfSymbol = '/';
const char LimiterOfQualifier = ':';
const char MultilineCommentEnd = '}';
const char HorizontalTabulation = '\t';
const char MultilineCommentBegin = '{';
const char SingleLineCommentBegin = '*';
// strings replacement in processing
const char UniversalSeparatorOfTokens = NullByte;

const int HorizontalTabulationSize = 8;
const int FirstLineNumber = 1;
const int FirstCharacterNumber = 1;

static bool IsSpace( char c )
{
	return ( c == Blank || c == HorizontalTabulation );
}

static bool IsOctal( char c )
{
	return ( c >= '0' && c <= '7' );
}

static bool IsDigit( char c )
{
	return ( c >= '0' && c <= '9' );
}

static bool IsAlpha( char c )
{
	return ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) );
}

static bool IsFirstWordLetter( char c )
{
	return ( c == Underline || IsAlpha( c ) );
}

static bool IsWordLetter( char c )
{
	return ( IsFirstWordLetter( c ) || IsDigit( c ) || c == Hyphen );
}

static bool IsControl( char c )
{
	return ( c == '\x7f' /* DEL */
		|| ( c >= '\0' && c < '\x20' && c != HorizontalTabulation
			&& c != CarriageReturn && c != LineFeed ) );
}

CScanner::CScanner( IErrorHandler* errorProcessor ):
	CParser( errorProcessor )
{
	Reset();
}

void CScanner::Reset()
{
	CParser::Reset();
	line = FirstLineNumber;
	position = FirstCharacterNumber;
	afterCarriageReturn = false;
	preprocessingState = PS_Initial;
	state = S_Initial;
}

void CScanner::AddChar( char c )
{
	normalizeLineFeed( c );
}

void CScanner::AddEndOfFile()
{
	preprocessingEndOfFile();
	Reset();
}

void CScanner::error( TErrorCode errorCode, char c )
{
	std::ostringstream errorStream;
	errorStream << line << ":" << position << ": error: ";
	switch( errorCode ) {
		case E_InvalidCharacter:
			errorStream << "invalid character `" << c << "`";
			break;
		case E_InvalidControlCharacter:
			errorStream << "invalid control character with ASCII code "
				<< "`" << static_cast<int>( c ) << "`";
			break;
		case E_UnexpectedCharacter:
			errorStream << "unexpected character `" << c << "`";
			break;
		case E_UnclosedMultilineCommentAtTheEndOfFile:
			errorStream << "unclosed multiline comment at the end of file";
			break;
		case E_UnclosedString:
			errorStream << "unclosed string";
			break;
		case E_UnclosedLabel:
			errorStream << "unclosed label";
			break;
		case E_UnexpectedCharacterInLabel:
			errorStream << "unexpected character in label";
			break;
		case E_UnclosedNumber:
			errorStream << "unclosed number";
			break;
		case E_UnclosedQualifier:
			errorStream << "unclosed qualifier";
			break;
		case E_UnexpectedCharacterInQualifier:
			errorStream << "unexpected character in qualifier";
			break;
		default:
			assert( false );
			break;
	}
	errorStream << ".";
	CErrorsHelper::Error( errorStream.str() );
}

void CScanner::setLineAndPositionOfToken()
{
	token.line = line;
	token.position = position;
}

void CScanner::addToken( TTokenType tokenType )
{
	token.type = tokenType;
	CParser::AddToken();
}

void CScanner::addTokenWithCurrentLineAndPosition( TTokenType tokenType )
{
	setLineAndPositionOfToken();
	addToken( tokenType );	
}

void CScanner::normalizeLineFeed( char c )
{
	if( c == CarriageReturn ) {
		preprocessing( LineFeed );
		line++;
		position = FirstCharacterNumber;
	} else if( c == LineFeed ) {
		if( !afterCarriageReturn ) {
			preprocessing( LineFeed );
			line++;
			position = FirstCharacterNumber;
		}
	} else if( IsControl( c ) ) {
		error( E_InvalidControlCharacter, c );
		position++;
	} else {
		preprocessing( c );
		position++;
		if( c == HorizontalTabulation ) {
			while( ( position - FirstCharacterNumber )
				% HorizontalTabulationSize != 0 )
			{
				position++;
			}
		}
	}
	afterCarriageReturn = ( c == CarriageReturn );
}

void CScanner::preprocessing( char c )
{
	switch( preprocessingState ) {
		case PS_Initial:
			preprocessingInitital( c );
			break;
		case PS_Plus:
			preprocessingPlus( c );
			break;
		case PS_PlusAfterLineFeed:
			preprocessingPlusAfterLineFeed( c );
			break;
		case PS_PlusAfterMultilineComment:
			preprocessingPlusAfterMultilineComment( c );
			break;
		case PS_PlusAfterMultilineCommentAndLineFeed:
			preprocessingPlusAfterMultilineCommentAndLineFeed( c );
			break;
		case PS_SingleLineComment:
			preprocessingSingleLineComment( c );
			break;
		case PS_MultilineComment:
			preprocessingMultilineComment( c );
			break;
		case PS_String:
			preprocessingString( c );
			break;
		case PS_StringAfterQuote:
			preprocessingStringAfterQuote( c );
			break;
		case PS_StringAfterBackslash:
			preprocessingStringAfterBackslash( c );
			break;
		case PS_StringOctalCodeOne:
			preprocessingStringOctalCodeOne( c );
			break;
		case PS_StringOctalCodeTwo:
			preprocessingStringOctalCodeTwo( c );
			break;
		default:
			assert( false );
			break;
	}
}

void CScanner::preprocessingEndOfFile()
{
	switch( preprocessingState ) {
		case PS_MultilineComment:
		case PS_PlusAfterMultilineComment:
		case PS_PlusAfterMultilineCommentAndLineFeed:
			error( E_UnclosedMultilineCommentAtTheEndOfFile );
			preprocessingState = PS_Initial;
			break;
		case PS_Plus:
		case PS_PlusAfterLineFeed:
			preprocessingState = PS_Initial;
			break;
		case PS_Initial:
		case PS_SingleLineComment:
		case PS_String:
		case PS_StringAfterQuote:
		case PS_StringAfterBackslash:
		case PS_StringOctalCodeOne:
		case PS_StringOctalCodeTwo:
			break;
		default:
			assert( false );
			break;
	}
	preprocessing( LineFeed );
}

void CScanner::preprocessingInitital( char c )
{
	switch( c ) {
		case Plus:
			preprocessingState = PS_Plus;
			break;
		case Quote:
			processing( UniversalSeparatorOfTokens );
			setLineAndPositionOfToken();
			token.value.string.clear();
			preprocessingState = PS_String;
			break;
		case SingleLineCommentBegin:
			preprocessingState = PS_SingleLineComment;
			break;
		case MultilineCommentBegin:
			preprocessingState = PS_MultilineComment;
			break;
		default:
			processing( c );
			break;
	}
}

void CScanner::preprocessingPlus( char c )
{
	if( c == LineFeed ) {
		preprocessingState = PS_PlusAfterLineFeed;
	} else if( c == MultilineCommentBegin ) {
		preprocessingState = PS_PlusAfterMultilineComment;
	}
}

void CScanner::preprocessingPlusAfterLineFeed( char c )
{
	if( !IsSpace( c ) && c != LineFeed ) {
		preprocessingState = PS_Initial;
		preprocessing( c );
	}
}

void CScanner::preprocessingPlusAfterMultilineComment( char c )
{
	if( c == MultilineCommentEnd ) {
		preprocessingState = PS_Plus;
	} else if( c == LineFeed ) {
		preprocessingState = PS_PlusAfterMultilineCommentAndLineFeed;
	}
}

void CScanner::preprocessingPlusAfterMultilineCommentAndLineFeed( char c )
{
	if( c == MultilineCommentEnd ) {
		preprocessingState = PS_PlusAfterLineFeed;
	}
}

void CScanner::preprocessingSingleLineComment( char c )
{
	if( c == LineFeed ) {
		processing( LineFeed );
		preprocessingState = PS_Initial;
	}
}
void CScanner::preprocessingMultilineComment( char c )
{
	if( c == LineFeed ) {
		processing( LineFeed );
	} else if( c == MultilineCommentEnd ) {
		preprocessingState = PS_Initial;
	}
}

void CScanner::preprocessingString( char c )
{
	switch( c ) {
		case Quote:
			if( token.value.string.empty() ) {
				token.value.string.push_back( Quote );
				addToken( TT_String );
				preprocessingState = PS_Initial;
			} else {
				preprocessingState = PS_StringAfterQuote;
			}
			break;
		case Backslash:
			preprocessingState = PS_StringAfterBackslash;
			break;
		case LineFeed:
			if( token.value.string.empty() ) {
				error( E_UnexpectedCharacter );
			} else {
				error( E_UnclosedString );
				addToken( TT_String );
			}
			preprocessingState = PS_Initial;
			preprocessing( LineFeed );
			break;
		default:
			token.value.string.push_back( c );
			break;
	}
}

void CScanner::preprocessingStringAfterQuote( char c )
{
	if( c == Quote ) {
		token.value.string.push_back( Quote );
		preprocessingState = PS_String;
	} else {
		assert( !token.value.string.empty() );
		addToken( TT_String );
		preprocessingState = PS_Initial;
		preprocessing( c );
	}
}

void CScanner::preprocessingStringAfterBackslash( char c )
{
	preprocessingState = PS_String;
	switch( c ) {
		case 'n':
			token.value.string.push_back( LineFeed );
			break;
		case 't':
			token.value.string.push_back( HorizontalTabulation );
			break;
		case 'v':
			token.value.string.push_back( '\v' );
			break;
		case 'b':
			token.value.string.push_back( '\b' );
			break;
		case 'r':
			token.value.string.push_back( CarriageReturn );
			break;
		case 'f':
			token.value.string.push_back( '\f' );
			break;
		case Backslash:
			token.value.string.push_back( Backslash );
			break;
		default:
			if( IsOctal( c ) ) {
				octalCodeOne = c;
				preprocessingState = PS_StringOctalCodeOne;
			} else {
				token.value.string.push_back( Backslash );
				preprocessing( c );
			}
			break;
	}
}

void CScanner::preprocessingStringOctalCodeOne( char c )
{
	if( IsOctal( c ) ) {
		octalCodeTwo = c;
		preprocessingState = PS_StringOctalCodeTwo;
	} else {
		if( octalCodeOne == Zero ) {
			token.value.string.push_back( NullByte );
		} else {
			token.value.string.push_back( Backslash );
			token.value.string.push_back( octalCodeOne );
		}
		token.value.string.push_back( c );
		preprocessingState = PS_String;
	}
}

void CScanner::preprocessingStringOctalCodeTwo( char c )
{
	if( IsOctal( c ) ) {
		int code = ( ( octalCodeOne - Zero ) * 8 +
			( octalCodeTwo - Zero ) ) * 8 + ( c - Zero );
		token.value.string.push_back( static_cast<char>( code ) );
	} else {
		token.value.string.push_back( Backslash );
		token.value.string.push_back( octalCodeOne );
		token.value.string.push_back( octalCodeTwo );
		token.value.string.push_back( c );
	}
	preprocessingState = PS_String;
}

void CScanner::processing( char c )
{
	//printf( "%c", c );
	//printf( "%4d:%3d:%c\n", line, position, c );
	switch( state )	{
		case S_Initial:
			processingInitial( c );
			break;
		case S_Blank:
			processingBlank( c );
			break;
		case S_Symbol:
			processingSymbol( c );
			break;
		case S_Label:
			processingLabel( c );
			break;
		case S_Number:
			processingNumber( c );
			break;
		case S_Word:
			processingWord( c );
			break;
		case S_BeginOfQualifier:
			processingBeginOfQualifier( c );
			break;
		case S_Qualifier:
			processingQualifier( c );
			break;
		default:
			assert( false );
			break;
	}
}

void CScanner::processingInitial( char c )
{
	switch( c ) {
		case Comma:
			addTokenWithCurrentLineAndPosition( TT_Comma );
			break;
		case Equal:
			addTokenWithCurrentLineAndPosition( TT_Equal );
			break;
		case LineFeed:
			addTokenWithCurrentLineAndPosition( TT_LineFeed );
			break;
		case LeftParen:
			addTokenWithCurrentLineAndPosition( TT_LeftParen );
			break;
		case RightParen:
			addTokenWithCurrentLineAndPosition( TT_RightParen );
			break;
		case LeftBracket:
			addTokenWithCurrentLineAndPosition( TT_LeftBracket );
			break;
		case Dot:
		case RightBracket:
			token.value.word = c;
			addTokenWithCurrentLineAndPosition( TT_RightBracket );
			break;
		case LimiterOfSymbol:
			setLineAndPositionOfToken();
			state = S_Symbol;
			break;
		case LimiterOfQualifier:
			setLineAndPositionOfToken();
			state = S_BeginOfQualifier;
			break;
		case UniversalSeparatorOfTokens:
			break;
		default:
			if( IsSpace( c ) ) {
				setLineAndPositionOfToken();
				state = S_Blank;
			} else if( c == Hyphen ) {
				error( E_UnexpectedCharacter, c );
			} else if( IsWordLetter( c ) ) {
				token.value.word = c;
				setLineAndPositionOfToken();
				state = S_Word;
			} else {
				error( E_InvalidCharacter, c );
			}
			break;
	}
}

void CScanner::processingBlank( char c )
{
	if( !IsSpace( c ) ) {
		if( c != LineFeed ) {
			addToken( TT_Blank );
		}
		state = S_Initial;
		processing( c );
	}
}

void CScanner::processingSymbol( char c )
{
	if( IsFirstWordLetter( c ) ) {
		token.value.word = c;
		state = S_Label;
	} else if( IsDigit( c ) ) {
		token.value.number = ( c - Zero );
		state = S_Number;
	} else {
		error( E_UnexpectedCharacterInLabel );
		state = S_Initial;
		processing( c );
	}
}
void CScanner::processingLabel( char c )
{
	if( IsWordLetter( c ) ) {
		token.value.word += c;
	} else if( c == LimiterOfSymbol ) {
		addToken( TT_Label );
		state = S_Initial;
	} else {
		error( E_UnclosedLabel );
		state = S_Initial;
		processing( c );
	}
}
void CScanner::processingNumber( char c )
{
	if( IsDigit( c ) ) {
		token.value.number = token.value.number * 10 + ( c - Zero );
		// TODO: check overflow
	} else if( c == LimiterOfSymbol ) {
		addToken( TT_Number );
		state = S_Initial;
	} else {
		error( E_UnclosedNumber );
		state = S_Initial;
		processing( c );
	}
}
void CScanner::processingWord( char c )
{
	if( IsWordLetter( c ) ) {
		token.value.word += c;
	} else {
		addToken( TT_Word );
		state = S_Initial;
		processing( c );
	}
}

void CScanner::processingBeginOfQualifier( char c )
{
	if( IsFirstWordLetter( c ) ) {
		token.value.word = c;
		state = S_Qualifier;
	} else {
		error( E_UnexpectedCharacterInQualifier );
		state = S_Initial;
		processing( c );
	}
}
void CScanner::processingQualifier( char c )
{
	if( IsWordLetter( c ) ) {
		token.value.word += c;
	} else if( c == LimiterOfQualifier ) {
		addToken( TT_Qualifier );
		state = S_Initial;
	} else {
		error( E_UnclosedQualifier );
		state = S_Initial;
		processing( c );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
