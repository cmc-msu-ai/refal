#include <Refal2.h>

namespace Refal2 {

//----------	-------------------------------------------------------------------
// CParser

const char* const QualifierTag = "s";

CParser::CParser( IErrorHandler* errorHandler ) :
	CDirectiveParser( errorHandler )
{
	Reset();
}

void CParser::Reset()
{
	CDirectiveParser::Reset();
	state = &CParser::parsingInitial;
}

void CParser::AddToken()
{
	CError::ResetSeverity();
	CError::SetErrorPosition( token.line, token.position, token.word );
	( this->*state )();
}

void CParser::parsingInitial()
{
	assert( token.type != TT_None );
	if( token.type == TT_Word ) {
		CRuleParser::EndFunction(); // action
		token.Move( savedToken1 );
		state = &CParser::parsingWord;
	} else if( token.type == TT_Blank ) {
		state = &CParser::parsingBlank;
	} else if( token.type != TT_LineFeed ) {
		CError::SetSeverity( ES_Error );
		CError::SetMessage( "line should begin with word or space" );
		CErrorsHelper::Error();
		state = &CParser::parsingIgnoreLine;
	}
}

void CParser::parsingIgnoreLine()
{
	if( token.type == TT_LineFeed ) {
		state = &CParser::parsingInitial;
	}
}

void CParser::parsingWord()
{
	if( token.type == TT_LineFeed ) {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		state = &CParser::parsingInitial;
	} else if( token.type == TT_Blank ) {
		state = &CParser::parsingWordBlank;
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = &CParser::parsingRule;
		AddToken();
	}
}

void CParser::parsingWordBlank()
{
	if( token.type == TT_LineFeed ) {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		state = &CParser::parsingInitial;
		return;
	}
	if( token.type == TT_Word ) {
		if( CDirectiveParser::StartParseIfStartDirective( savedToken1 ) ) {
			state = &CParser::parsingDirective;
			return;
		}
		if( CompareNoCase( token.word, QualifierTag ) ) {
			token.Move( savedToken2 );
			state = &CParser::parsingWordBlankS;
			return;
		}
	}
	token.Swap( savedToken1 );
	CRuleParser::BeginFunction(); // action
	token.Swap( savedToken1 );
	state = &CParser::parsingRule;
	AddToken();
}

void CParser::parsingWordBlankS()
{
	if( token.type == TT_Blank ) {
		token.Swap( savedToken1 );
		CQualifierParser::StartNamedQualifier();
		state = IsWrong() ?
			&CParser::parsingIgnoreLine : &CParser::parsingQualifier;
		token.Swap( savedToken1 );
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = &CParser::parsingRule;
		savedToken2.Swap( token );
		AddToken(); // QualifierTag
		savedToken2.Move( token );
		AddToken(); // current token
	}
}

void CParser::parsingBlank()
{
	if( token.type == TT_Word && CDirectiveParser::StartParseIfDirective() ) {
		state = &CParser::parsingDirective;
	} else if( token.type == TT_LineFeed ) {
		state = &CParser::parsingInitial;
	} else {
		CRuleParser::BeginRule();
		state = &CParser::parsingRule;
		AddToken();
	}
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
		state = &CParser::parsingInitial;
	} else if( IsWrong() ) {
		state = &CParser::parsingIgnoreLine;
		AddToken();
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
