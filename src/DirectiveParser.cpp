#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TDirectiveType {
	DT_None,
	DT_Start,
	DT_End,
	DT_Empty,
	DT_External,
	DT_Entry
};

struct TDirectiveKindTag {
	TDirectiveType Type;
	const char* const Tag;
};

const TDirectiveKindTag Directives[] = {
	{ DT_Start, "start" },
	{ DT_End, "end" },
	{ DT_Empty, "empty" },
	{ DT_External, "extrn" },
	{ DT_Entry, "entry" },
	{ DT_None, 0 }
};

static TDirectiveType findDirective( const std::string& _tag )
{
	std::string tag( _tag );
	MakeLower( tag );
	int i = 0;
	while( Directives[i].Tag != 0 ) {
		if( tag == Directives[i].Tag ) {
			break;
		}
		i++;
	}
	return Directives[i].Type;
}

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
	if( findDirective( token.word ) == DT_Start ) {
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
	TDirectiveType directiveType = findDirective( token.word );
	if( directiveType != DT_None ) {
		CRuleParser::EndFunction();
		CParsingElementState::Reset();
		token.Move( directive );
		switch( directiveType ) {
			case DT_Start:
				state = S_Simple;
				CModuleBuilder::StartModule( directive );
				break;
			case DT_End:
				state = S_Simple;
				CModuleBuilder::EndModule( directive );
				break;
			case DT_Empty:
				state = S_OneName;
				handler = &CDirectiveParser::addEmpty;
				break;
			case DT_External:
				state = S_TwoNames;
				handler = &CDirectiveParser::addExternal;
				break;
			case DT_Entry:
				state = S_TwoNames;
				handler = &CDirectiveParser::addEntry;
				break;
			case DT_None:
			default:
				assert( false );
				break;
		}
		return true;
	}
	return false;
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
	CErrorsHelper::RaiseError( ES_Error, "wrong `" + directive.word
		+ "` directive format", token );
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
