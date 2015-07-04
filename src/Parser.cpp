#include <Refal2.h>

namespace Refal2 {

CParser::CParser( IErrorHandler* errorHandler ):
	CErrorsHelper( errorHandler ),
	ruleParser( errorHandler ),
	qualifierParser( errorHandler )
{
	Reset();
}

void CParser::Reset()
{
	ruleParser.Reset();
	qualifierParser.Reset();
	state = S_Initial;
}

bool CParser::wordIs( const std::string& value ) const
{
	if( token.type == TT_Word ) {
		std::string lowerWord( token.word );
		MakeLower( lowerWord );
		return ( lowerWord == value );
	}
	return false;
}

void CParser::AddToken()
{
	switch( state ) {
		case S_Initial:
			parsingInitial();
			break;
		case S_IgnoreLine:
			parsingIgnoreLine();
			break;		
		case S_Word:
			parsingWord();
			break;
		case S_WordBlank:
			parsingWordBlank();
			break;
		case S_WordBlankS:
			parsingWordBlankS();
			break;
		case S_Blank:
			parsingBlank();
			break;
		case S_Directive:
			parsingDirective();
			break;
		case S_Qualifier:
			parsingQualifier();
			break;
		case S_Rule:
			parsingRule();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::parsingInitial()
{
	if( token.type == TT_Word ) {
		ruleParser.EndFunction(); // action
		token.Move( savedToken1 );
		state = S_Word;
	} else if( token.type == TT_Blank ) {
		state = S_Blank;
	} else if( token.type != TT_LineFeed ) {
		ruleParser.EndFunction(); // action
		error( EC_LineShouldBeginWithIdentifierOrSpace );
		state = S_IgnoreLine;
	}
}

void CParser::parsingIgnoreLine()
{
	if( token.type == TT_LineFeed ) {
		state = S_Initial;
	}
}

void CParser::parsingWord()
{
	if( token.type == TT_LineFeed ) {
		ruleParser.BeginFunction( savedToken1 ); // action
		state = S_Initial;
	} else if( token.type == TT_Blank ) {
		state = S_WordBlank;
	} else {
		ruleParser.BeginFunction( savedToken1 ); // action
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingWordBlank()
{
	if( wordIs( "start" ) ) {
		state = S_Directive;
	} else if( wordIs( "s" ) ) {
		state = S_WordBlankS;
		token.Move( savedToken2 );
	} else {
		ruleParser.BeginFunction( savedToken1 ); // action
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingWordBlankS()
{
	if( token.type == TT_Blank ) {
		if( qualifierParser.StartNamedQualifier( savedToken1 ) ) {
			state = S_Qualifier;
		} else {
			state = S_IgnoreLine;
		}
	} else {
		ruleParser.BeginFunction( savedToken1 ); // action
		state = S_Rule;
		savedToken2.Swap( token );
		AddToken(); // "S"
		savedToken2.Move( token );
		AddToken(); // current token
	}
}

void CParser::parsingBlank()
{
	if( token.type == TT_Word ) {
		if( wordIs( "start" )
			|| wordIs( "end" )
			|| wordIs( "entry" )
			|| wordIs( "empty" )
			|| wordIs( "swap" )
			|| wordIs( "extrn" ) )
		{
			ruleParser.EndFunction(); // action
			state = S_Directive;
			return;
		}
	}
	ruleParser.BeginRule();
	state = S_Rule;
	AddToken();
}

void CParser::parsingDirective()
{
	std::ostringstream errorStream;
	errorStream << ":" << token.line << ":" << token.position
		<< ": error in directive: " << "not implemented yet" << ".";
	CErrorsHelper::Error( errorStream.str() );
	state = S_IgnoreLine;
}

void CParser::parsingQualifier()
{
	if( qualifierParser.AddToken( token ) ) {
		if( qualifierParser.HasErrors() ) {
			state = S_IgnoreLine;
			AddToken();
		} else {
			state = S_Initial;
		}
	}
}

void CParser::parsingRule()
{
	if( ruleParser.AddToken( token ) ) {
		if( ruleParser.HasErrors() ) {
			state = S_IgnoreLine;
			AddToken();
		} else {
			state = S_Initial;
		}
	}
}

void CParser::error( TErrorCode errorCode )
{
	std::ostringstream errorStream;
	errorStream << token.line << ":" << token.position << ": error: ";
	switch( errorCode ) {
		case EC_LineShouldBeginWithIdentifierOrSpace:
			errorStream << "line should begin with identifier or space";
			break;
		case EC_NewLineExpected:
			errorStream << "line feed expected";
			break;
		case EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine:
			errorStream << "unexpected token after identifier in the beginning of the line";
		case EC_STUB:
			errorStream << "stub" ;
			break;
		default:
			assert( false );
			break;
	}
	errorStream << ".";
	CErrorsHelper::Error( errorStream.str() );
}

} // end of namespace Refal2
