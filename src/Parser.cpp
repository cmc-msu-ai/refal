#include <iostream>
#include <Refal2.h>

namespace Refal2 {

void CParser::Reset()
{
	state = PS_Begin;
}

void CParser::ProcessLexem()
{
	switch( state )
	{
		case PS_Begin:
			if( lexem == L_Identificator ) {
				state = PS_BeginIdent;
				storedName = ToLower( lexemString ); // action
				addEndOfFunction(); // action
			} else if( lexem == L_Blank ) {
				state = PS_BeginBlank;
			} else {
				state = PS_WaitNewline;
				addEndOfFunction(); // action
				/* TODO: ERROR */
			}
			break;
		case PS_OnlyNewline:
			if( lexem == L_Newline ) {
				state = PS_Begin;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_WaitNewline:
			if( lexem == L_Newline ) {
				state = PS_Begin;
			}
			break;
		case PS_BeginIdent:
			if( lexem == L_Newline ) {
				state = PS_Begin;
				addEndOfFunction(); // action
			} else if( lexem == L_Blank ) {
				state = PS_BeginIdentBlank;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginIdentBlank:
			if( identificatorIs( "start" ) ) {
				state = PS_OnlyNewline;
				/* TODO: WARNING */
			} else if( identificatorIs( "s" ) ) {
				state = PS_BeginIdentBlankS;
				storedOffset = offset;
			} else {
				addDeclarationOfFunction( storedName ); // action
				state = PS_ProcessRule;
				ProcessLexem();
			}
			break;
		case PS_BeginIdentBlankS:
			if( lexem == L_Blank ) {
				addDeclarationOfQualifier( storedName ); // action
				state = PS_ProcessQualifier;
			} else {
				addDeclarationOfFunction( storedName ); // action

				TLexem tmpLexem = lexem;
				state = PS_ProcessRule;
					
				std::string tmpLexemString( "S" );
				lexemString.swap( tmpLexemString );

				std::swap( offset, storedOffset );

				lexem = L_Identificator;
				ProcessLexem();
			
				lexemString.swap( tmpLexemString );
				std::swap( offset, storedOffset );

				lexem = tmpLexem;
				ProcessLexem();
			}
			break;
		case PS_BeginBlank:
			if( identificatorIs( "end" ) ) {
				state = PS_OnlyNewline;
				addEndOfFunction(); // action
				/* TODO: WARNING */
			} else if( identificatorIs( "extrn" ) ) {
				state = PS_BeginExtrn;
				addEndOfFunction(); // action
			} else if( identificatorIs( "empty" ) ) {
				state = PS_BeginEmpty;
				addEndOfFunction(); // action
			} else if( identificatorIs( "entry" ) ) {
				state = PS_BeginEntry;
				addEndOfFunction(); // action
			} else {
				state = PS_ProcessRule;
				ProcessLexem();
			}
			break;
		/* entry */
		case PS_BeginEntry:
			if( lexem == L_Blank ) {
				state = PS_BeginEntryBlank;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginEntryBlank:
			if( lexem == L_Identificator ) {
				state = PS_OnlyNewline;
				addEntryFunction(ToLower(lexemString) ); // action
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		/* end of entry */
		/* empty */
		case PS_BeginEmpty:
			if( lexem == L_Blank ) {
				state = PS_BeginEmptyComma;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginEmptyComma:
			if( lexem == L_Identificator ) {
				state = PS_BeginEmptyIdent;
				addEmptyFunction( ToLower(lexemString) ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginEmptyIdent:
			if( lexem == L_Newline ) {
				state = PS_Begin;
			} else if( lexem == L_Comma ) {
				state = PS_BeginEmptyComma;
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		/* end of empty */
		/* extrn */
		case PS_BeginExtrn:
			if( lexem == L_Blank ) {
				state = PS_BeginExtrnComma;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginExtrnComma:
			if( lexem == L_Identificator ) {
				state = PS_BeginExtrnIdent;
				storedName = ToLower( lexemString ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginExtrnIdent:
			if( lexem == L_Newline ) {
				state = PS_Begin;
				addExtrnFunction( storedName, storedName ); // action
			} else if( lexem == L_Comma ) {
				state = PS_BeginExtrnComma;
				addExtrnFunction( storedName, storedName ); // action
			} else if( lexem == L_LeftParen ) {
				state = PS_BeginExtrnLeftParen;
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginExtrnLeftParen:
			if( lexem == L_Identificator ) {
				state = PS_BeginExtrnInnerIdent;
				addExtrnFunction( storedName, ToLower( lexemString ) ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginExtrnInnerIdent:
			if( lexem == L_RightParen ) {
				state = PS_BeginExtrnOnlyComma;
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		case PS_BeginExtrnOnlyComma:
			if( lexem == L_Comma ) {
				state = PS_BeginExtrnComma;
			} else if( lexem == L_Newline ) {
				state = PS_Begin;
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */
			}
			break;
		/* end of extrn */
		case PS_ProcessQualifier:
			processQualifier();
			break;
		case PS_ProcessRule:
			processRule();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processRule()
{
	if( functionBuilder.IsDirectionState() ) {
		if( lexem == L_Blank ) {
			return;
		} else if( identificatorIs("l") ) {
			functionBuilder.AddDirection();
			return;
		} else if( identificatorIs("r") ) {
			functionBuilder.AddDirection( true /* isRightDirection */ );
			return;
		} else {
			functionBuilder.AddDirection();
			// TODO: error
		}
	}

	switch( lexem ) {
		case L_Blank:
			break;
		case L_Equal:
			// TODO: functionBuilder.AddEndOfLeft;
			break;
		case L_Comma:
			// TODO: Error
			break;
		case L_Label:
			// TODO: functionBuilder.AddLabel( lexemLabel );
			break;
		case L_Number:
			// TODO: functionBuilder.AddNumber( lexemNumber );
			break;
		case L_String:
			for( std::size_t i = 0; i < lexemString.size(); i++ ) {
				// TODO: functionBuilder.AddChar( lexemString[i] );
			}
			break;
		case L_Qualifier:
			//std::cout << ":" << lexemString << ": ";
			break;
		case L_LeftParen:
			// TODO:
			break;
		case L_RightParen:
			// TODO: functionBuilder.AddRightParen();
			break;
		case L_LeftBracket:
			// TODO: functionBuilder.AddLeftBracket();
			break;
		case L_RightBracket:
			// TODO: functionBuilder.AddRightBracket();
			break;
		case L_Identificator:
			// TODO:
			break;
		case L_Newline:
			// TODO: functionBuilder.AddEndOfRight();
			state = PS_Begin;
			break;
		case L_EndOfFile:
			// TODO: functionBuilder.AddEndOfRight();
			state = PS_Begin;
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processQualifier()
{
}

void CParser::addDeclarationOfFunction(const std::string& name)
{
}

void CParser::addEndOfFunction()
{
}

void CParser::addDeclarationOfQualifier(const std::string& name)
{
}

void CParser::addEmptyFunction(const std::string& name)
{
	std::cout << "Empty: " << name << "\n";
}

void CParser::addEntryFunction(const std::string& name)
{
	std::cout << "Entry: " << name << "\n";
}

void CParser::addExtrnFunction(const std::string& name,
	const std::string& oldName)
{
	std::cout << "Extrn: " << name << "(" << oldName << ")\n";
}

} // end of namespace Refal2
