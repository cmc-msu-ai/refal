#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const char* StartDirectiveTag = "start";
const char* EndDirectiveTag = "end";
const char* EmptyDirectiveTag = "empty";
const char* ExternalDirectiveTag = "extrn";
const char* EntryDirectiveTag = "entry";

CDirectiveParser::CDirectiveParser( IErrorHandler* errorHandler ):
	CRuleParser( errorHandler )
{
	Reset();
}

void CDirectiveParser::Reset()
{
	CRuleParser::Reset();
	handler = 0;
}

bool CDirectiveParser::StartParseIfStartDirective( const CToken& moduleName )
{
	assert( token.type == TT_Word );
	std::string word = token.word;
	MakeLower( word );
	if( word == StartDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_Simple;
		token.Move( directive );
		CModuleBuilder::StartModule( token, moduleName );
		return ( !IsWrong() );
	}
	return false;
}

bool CDirectiveParser::StartParseIfDirective()
{
	assert( token.type == TT_Word );
	std::string word = token.word;
	MakeLower( word );
	if( word == StartDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_Simple;
		token.Move( directive );
		CModuleBuilder::StartModule( directive );
	} else if( word == EndDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_Simple;
		token.Move( directive );
		CModuleBuilder::EndModule( directive );
	} else if( word == EmptyDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_OneName;
		token.Move( directive );
		handler = &CDirectiveParser::addEmpty;
	} else if( word == ExternalDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_TwoNames;
		token.Move( directive );
		handler = &CDirectiveParser::addExternal;
	} else if( word == EntryDirectiveTag ) {
		CParsingElementState::Reset();
		state = S_TwoNames;
		token.Move( directive );
		handler = &CDirectiveParser::addEntry;
	} else {
		return false;
	}
	return true;
}

void CDirectiveParser::AddToken()
{
	switch( state ) {
		case S_Simple:
			simple();
			break;
		case S_OneName:
			oneName();
			break;
		case S_OneNameAfterName:
			oneNameAfterName();
			break;
		case S_TwoNames:
			twoNames();
			break;
		case S_TwoNamesAfterName:
			twoNamesAfterName();
			break;
		case S_TwoNamesAfterLeftParen:
			twoNamesAfterLeftParen();
			break;
		case S_TwoNamesAfterAlias:
			twoNamesAfterAlias();
			break;
		case S_TwoNamesAfterRightParen:
			twoNamesAfterRightParen();
			break;
		default:
			assert( false );
			break;
	}
}

void CDirectiveParser::wrongDirectiveFormat()
{
	SetWrong();
	CErrorsHelper::Error( "wrong `" + directive.word + "` directive format." );
}

void CDirectiveParser::simple()
{
	if( token.type == TT_LineFeed ) {
		SetCorrect();
	} else {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::oneName()
{
	if( token.type == TT_Word ) {
		( this->*handler )();
		state = S_OneNameAfterName;
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::oneNameAfterName()
{
	if( token.type == TT_Comma ) {
		state = S_OneName;
	} else if( token.type == TT_LineFeed ) {
		SetCorrect();
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	} 
}

void CDirectiveParser::twoNames()
{
	if( token.type == TT_Word ) {
		token.Move( savedToken );
		state = S_TwoNamesAfterName;
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::twoNamesAfterName()
{
	if( token.type == TT_Comma ) {
		( this->*handler )();
		state = S_TwoNames;
	} else if( token.type == TT_LeftParen ) {
		state = S_TwoNamesAfterLeftParen;
	} else if( token.type == TT_LineFeed ) {
		( this->*handler )();
		SetCorrect();
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::twoNamesAfterLeftParen()
{
	if( token.type == TT_Word ) {
		( this->*handler )();
		state = S_TwoNamesAfterAlias;
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::twoNamesAfterAlias()
{
	if( token.type == TT_RightParen ) {
		state = S_TwoNamesAfterRightParen;
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::twoNamesAfterRightParen()
{
	if( token.type == TT_Comma ) {
		state = S_TwoNames;
	} else if( token.type == TT_LineFeed ) {
		SetCorrect();
	} else if( token.type != TT_Blank ) {
		wrongDirectiveFormat();
	}
}

void CDirectiveParser::addEmpty()
{
	assert( token.type == TT_Word );
	CModuleBuilder::SetEmpty( token );
}

void CDirectiveParser::addExternal()
{
	if( token.type == TT_Word ) {
		CModuleBuilder::SetExternal( savedToken, token );
	} else {
		assert( token.type == TT_Comma || token.type == TT_LineFeed );
		CModuleBuilder::SetExternal( savedToken );
	}
}

void CDirectiveParser::addEntry()
{
	if( token.type == TT_Word ) {
		CModuleBuilder::SetEntry( savedToken, token );
	} else {
		assert( token.type == TT_Comma || token.type == TT_LineFeed );
		CModuleBuilder::SetEntry( savedToken );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
