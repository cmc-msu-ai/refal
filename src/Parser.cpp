#include <Refal2.h>

namespace Refal2 {

//----------	-------------------------------------------------------------------

const char* const QualifierTag = "s";

CParser::CParser( IErrorHandler* errorHandler ):
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
	( this->*state )();
}

void CParser::parsingInitial()
{
	if( token.type == TT_Word ) {
		CRuleParser::EndFunction(); // action
		token.Move( savedToken1 );
		state = &CParser::parsingWord;
	} else if( token.type == TT_Blank ) {
		state = &CParser::parsingBlank;
	} else if( token.type != TT_LineFeed ) {
		CRuleParser::EndFunction(); // action
		CErrorsHelper::Error( "line should begin with word or space" );
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
		token.Swap( savedToken1 );
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
	if( token.type == TT_Word
		&& CDirectiveParser::StartParseIfStartDirective( savedToken1 ) )
	{
		state = &CParser::parsingDirective;
	} else if( wordIs( QualifierTag ) ) {
		token.Move( savedToken2 );
		state = &CParser::parsingWordBlankS;
	} else {
		token.Swap( savedToken1 );
		CRuleParser::BeginFunction(); // action
		token.Swap( savedToken1 );
		state = &CParser::parsingRule;
		AddToken();
	}
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

bool CParser::wordIs( const std::string& value ) const
{
	if( token.type == TT_Word ) {
		std::string lowerWord( token.word );
		MakeLower( lowerWord );
		return ( lowerWord == value );
	}
	return false;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
