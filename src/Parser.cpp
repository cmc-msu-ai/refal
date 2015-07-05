#include <Refal2.h>

namespace Refal2 {

//----------	-------------------------------------------------------------------

const char* QualifierTag = "s";

CParser::CParser( IErrorHandler* errorHandler ):
	CDirectiveParser( errorHandler )
{
	Reset();
}

void CParser::Reset()
{
	CRuleParser::Reset();
	state = S_Initial;
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
		case S_Qualifier:
			parsingQualifier();
			break;
		case S_Rule:
			parsingRule();
			break;
		case S_Directive:
			parsingDirective();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::parsingInitial()
{
	if( token.type == TT_Word ) {
		CRuleParser::EndFunction(); // action
		token.Move( savedToken1 );
		state = S_Word;
	} else if( token.type == TT_Blank ) {
		state = S_Blank;
	} else if( token.type != TT_LineFeed ) {
		CRuleParser::EndFunction(); // action
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
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = S_Initial;
	} else if( token.type == TT_Blank ) {
		state = S_WordBlank;
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingWordBlank()
{
	if( token.type == TT_Word
		&& CDirectiveParser::StartParseIfStartDirective( savedToken1.word ) )
	{
		state = S_Directive;
	} else if( wordIs( QualifierTag ) ) {
		token.Move( savedToken2 );
		state = S_WordBlankS;
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingWordBlankS()
{
	if( token.type == TT_Blank ) {
		token.Swap( savedToken1 );
		CQualifierParser::StartNamedQualifier();
		state = IsWrong() ? S_IgnoreLine : S_Qualifier;
		token.Swap( savedToken1 );
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = S_Rule;
		savedToken2.Swap( token );
		AddToken(); // QualifierTag
		savedToken2.Move( token );
		AddToken(); // current token
	}
}

void CParser::parsingBlank()
{
	if( token.type == TT_Word && CDirectiveParser::StartParseIfDirective() ) {
		CRuleParser::EndFunction(); // action
		state = S_Directive;
		return;
	}
	CRuleParser::BeginRule();
	state = S_Rule;
	AddToken();
}

void CParser::parsingQualifier()
{
	CQualifierParser::AddToken();
	checkFinished();
}

void CParser::parsingRule()
{
	CRuleParser::AddToken();
	checkFinished();
}

void CParser::parsingDirective()
{
	CDirectiveParser::AddToken();
	checkFinished();
}

void CParser::checkFinished()
{
	if( IsCorrect() ) {
		state = S_Initial;
	} else if( IsWrong() ) {
		state = S_IgnoreLine;
		AddToken();
	}
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

void CParser::error( TErrorCode errorCode )
{
	switch( errorCode ) {
		case EC_LineShouldBeginWithIdentifierOrSpace:
			CErrorsHelper::Error( "line should begin with identifier or space" );
			break;
		case EC_NewLineExpected:
			CErrorsHelper::Error( "line feed expected" );
			break;
		case EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine:
			CErrorsHelper::Error( "unexpected token after identifier in the beginning of the line" );
		case EC_STUB:
			CErrorsHelper::Error( "stub" );
			break;
		default:
			assert( false );
			break;
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
