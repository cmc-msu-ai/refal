#include <string>
#include <algorithm>
#include <iostream>
#include <Refal2.h>

namespace Refal2 {

void CScanner::Reset()
{
	CFunctionBuilder::Reset();
	line = 1;
	offset = 0;
	lexemString.clear();
	state = SS_BeginOfLine;
	localOffset = 0;
}

void CScanner::ProcessLexem()
{
	switch( lexem ) {
		case L_Blank:
			std::cout << " ";
			break;
		case L_Equal:
			std::cout << "=";
			break;
		case L_Comma:
			std::cout << ",";
			break;
		case L_Label:
			std::cout << "/" << lexemString << "/";
			break;
		case L_Number:
			std::cout << "/" << lexemNumber << "/";
			break;
		case L_String:
			std::cout << "'" << lexemString << "'";
			break;
		case L_NewLine:
			std::cout << "\n";
			break;
		case L_EndOfFile:
			std::cout << "\nEndOfFile!\n";
			break;
		case L_Qualifier:
			std::cout << ":" << lexemString << ":";
			break;
		case L_LeftParen:
			std::cout << "(";
			break;
		case L_RightParen:
			std::cout << ")";
			break;
		case L_LeftBracket:
			std::cout << "<";
			break;
		case L_RightBracket:
			std::cout << ">";
			break;
		case L_Identificator:
			std::cout << lexemString;
			break;
		default:
			assert( false );
			break;
	}
}

void CScanner::AddChar(char c)
{
	static const int TabSize = 8;

	processChar(c);
	
	if( c == '\n' ) {
		line++;
		localOffset = 1;
	} else if( c == '\t' ) {
		while( localOffset % TabSize ) {
			localOffset++;
		}
	} else {
		localOffset++;
	}
}

std::string CScanner::ToLower(const std::string& data)
{
	std::string result( data );
	std::transform( result.begin(), result.end(), result.begin(), ::tolower );
	return result;
}

void CScanner::processChar( char c )
{
	if( c == '\r' ) {
	} else if( c == '\x7f' /* ASCII DEL - 127 */
		|| ( c != '\n' && c != '\t' && c >= '\0' && c <= '\x1f' ) )
	{
		error( SEC_UnexpectedControlSequence, c );
	} else {
		switch( state ) {
			case SS_BeginOfLine:
				if( c == ' ' || c == '\t' ) {
					state = SS_BeginBlank;
				} else if( c == '\n' || c == ';' ) {
					// TODO: warning if c == ';': old style
				} else if( c == '+' ) {
					state = SS_BeginPlus;
				} else if( c == '*' ) {
					state = SS_BeginComment;
				} else {
					state = SS_NotBeginOfLine;
					processChar( c );
				}
				break;
			case SS_BeginBlank:
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					state = SS_BeginComment;
				} else if( c == '+' ) {
					state = SS_BeginPlus;
				} else if( c == '\n' || c == ';' ) {
					// TODO: warning if c == ';': old style
					state = SS_BeginOfLine;
				} else {
					state = SS_NotBeginOfLine;
					offset = localOffset;
					lexem = L_Blank;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_BeginComment:
				if( c == '\n' ) {
					state = SS_BeginOfLine;
				}
				break;
			case SS_BeginPlus :
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					state = SS_BeginCommentAfterPlus;
				} else if( c == '\n' ) {
					state = SS_BeginOfLine;
				} else {
					error( SEC_SymbolAfterPlus, c );
				}
				break;
			case SS_BeginCommentAfterPlus:
				if( c == '\n' ) {
					state = SS_BeginOfLine;
				}
				break;
			case SS_NotBeginOfLine:
				if( c == '+' ) {
					state = SS_NotBeginPlus;
				} else if( c == '*' ) {
					state = SS_NotBeginComment;
				} else if( c == ' ' || c == '\t' ) {
					state = SS_NotBeginBlank;
				} else if( c == '\n' || c == ';' ) {
					// TODO: warning if c == ';': old style
					state = SS_BeginOfLine;
					offset = localOffset;
					lexem = L_NewLine;
					ProcessLexem();
				} else if( c == '\'' ) {
					state = SS_String;
					lexemString.clear();
					offset = localOffset;
				} else if( c == '/' ) {
					state = SS_BeginLabelOrNumber;
					offset = localOffset;
				} else if( c == ':' ) {
					state = SS_BeginQualifier;
				} else if( c == '(' ) {
					offset = localOffset;
					lexem = L_LeftParen;
					ProcessLexem();
				} else if( c == ')' ) {
					offset = localOffset;
					lexem = L_RightParen;
					ProcessLexem();
				} else if( c == '<' ) {
					offset = localOffset;
					lexem = L_LeftBracket;
					ProcessLexem();
				} else if( c == '>' || c == '.' ) {
					// TODO: warning if c == '.': old style
					offset = localOffset;
					lexem = L_RightBracket;
					ProcessLexem();
				} else if( c == '=' ) {
					offset = localOffset;
					lexem = L_Equal;
					ProcessLexem();
				} else if( c == ',' ) {
					offset = localOffset;
					lexem = L_Comma;
					ProcessLexem();
				} else if( isalnum( static_cast<unsigned char>( c ) )
					|| c == '_' )
				{
					state = SS_Identificator;
					lexemString = c;
					offset = localOffset;
				} else {
					error( SEC_UnexpectedCharacter, c );
				}
				break;
			case SS_NotBeginBlank:
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					state = SS_NotBeginComment;
				} else if( c == '+' ) {
					state = SS_NotBeginPlus;
				} else if( c == '\n' || c == ';' ) {
					// TODO: warning if c == ';': old style
					state = SS_BeginOfLine;
					offset = localOffset;
					lexem = L_NewLine;
					ProcessLexem();
				} else {
					state = SS_NotBeginOfLine;
					offset = localOffset;
					lexem = L_Blank;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_NotBeginComment:
				if( c == '\n' ) {
					state = SS_BeginOfLine;
					offset = localOffset;
					lexem = L_NewLine;
					ProcessLexem();
				}
				break;
			case SS_NotBeginPlus:
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					state = SS_NotBeginCommentAfterPlus;
				} else if( c == '\n' ) {
					state = SS_NotBeginBlank;
				} else {
					error( SEC_SymbolAfterPlus, c );
				}
				break;
			case SS_NotBeginCommentAfterPlus:
				if( c == '\n' ) {
					state = SS_NotBeginBlank;
				}
				break;
			case SS_BeginLabelOrNumber:
				if( isalpha( static_cast<unsigned char>( c ) ) || c == '_' ) {
					state = SS_NotBeginLabel;
					lexemString = c;
				} else if( isdigit( static_cast<unsigned char>( c ) ) ) {
					state = SS_NotBeginNumber;
					lexemNumber = c - '0';
				} else if( c == '\n' ) {
					error( SEC_UnclosedLabelOrNumber );
					state = SS_BeginOfLine;
				} else {
					error( SEC_IllegalCharacterInLabelOrNumberBegin, c );
					state = SS_NotBeginOfLine;
					processChar( c );
				}
				break;
			case SS_NotBeginLabel:
				if( isalnum( static_cast<unsigned char>( c ) )
					|| c == '_' || c == '-' )
				{
					lexemString += c;
				} else if( c == '/' ) {
					state = SS_NotBeginOfLine;
					lexem = L_Label;
					lexemLabel = LabelTable.AddLabel( ToLower( lexemString ) );
					ProcessLexem();
				} else if( c == '\n' ) {
					error( SEC_UnclosedLabel );
					state = SS_BeginOfLine;
					lexemLabel = LabelTable.AddLabel( ToLower( lexemString ) );
					lexem = L_Label;
					ProcessLexem();
				} else {
					error( SEC_IllegalCharacterInLabel, c );
					state = SS_NotBeginOfLine;
					lexemLabel = LabelTable.AddLabel( ToLower( lexemString ) );
					lexem = L_Label;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_NotBeginNumber:
				if( isdigit( static_cast<unsigned char>( c ) ) ) {
					lexemNumber *= 10;
					lexemNumber += c - '0';
				} else if( c == '/' ) {
					state = SS_NotBeginOfLine;
					lexem = L_Number;
					ProcessLexem();
				} else if( c == '\n' ) {
					error( SEC_UnclosedNumber );
					state = SS_BeginOfLine;
					lexem = L_Number;
					ProcessLexem();
				} else {
					error( SEC_IllegalCharacterInNumber, c );
					state = SS_NotBeginOfLine;
					lexem = L_Number;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_Identificator:
				if( isalnum( static_cast<unsigned char>( c ) )
					|| c == '_' || c == '-' )
				{
					lexemString += c;
				} else {
					state = SS_NotBeginOfLine;
					lexem = L_Identificator;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_BeginQualifier:
				if( isalpha( static_cast<unsigned char>( c ) ) || c == '_' ) {
					state = SS_NotBeginQualifier;
					lexemString = c;
				} else if( c == '\n' ) {
					error( SEC_UnclosedQualifier );
					state = SS_BeginOfLine;
				} else {
					error( SEC_IllegalCharacterInQualifierNameBegin, c );
					state = SS_NotBeginOfLine;
					processChar( c );
				}
				break;
			case SS_NotBeginQualifier:
				if( isalnum( static_cast<unsigned char>( c ) ) || c == '_' ) {
					lexemString += c;
				} else if( c == ':' ) {
					state = SS_NotBeginOfLine;
					lexem = L_Qualifier;
					ProcessLexem();
				} else if( c == '\n' ) {
					error( SEC_UnclosedQualifier );
					state = SS_BeginOfLine;
					lexem = L_Qualifier;
					ProcessLexem();
				} else {
					error( SEC_IllegalCharacterInQualifierName, c );
					state = SS_NotBeginOfLine;
					lexem = L_Qualifier;
					ProcessLexem();
					processChar( c );
				}
				break;
			case SS_String:
				if( c == '\'' ) {
					state = SS_StringAfterQuote;
				} else if( c == '\\' ) {
					state = SS_StringAfterBackslash;
				} else if( c == '\n' ) {
					error( SEC_UnclosedStringConstant );
					state = SS_BeginOfLine;
					lexem = L_String;
					ProcessLexem();
				} else {
					lexemString += c;
				}
				break;
			case SS_StringAfterQuote:
				if( c == '\'' ) {
					state = SS_String;
					lexemString += '\'';
				} else {
					lexem = L_String;
					ProcessLexem();
					state = SS_NotBeginOfLine;
					processChar( c );
				}
				break;
			case SS_StringAfterBackslash:
				if( c == 'n' ) {
					state = SS_String;
					lexemString += '\n';
				} else if( c == 't' ) {
					state = SS_String;
					lexemString += '\t';
				} else if( c == 'v' ) {
					state = SS_String;
					lexemString += '\v';
				} else if( c == 'b' ) {
					state = SS_String;
					lexemString += '\b';
				} else if( c == 'r' ) {
					state = SS_String;
					lexemString += '\r';
				} else if( c == 'f' ) {
					state = SS_String;
					lexemString += '\f';
				} else if( c == '\\' ) {
					state = SS_String;
					lexemString += '\\';
				} else if( c >= '0' && c <= '7' ) {
					state = SS_StringMayOctalCode;
					octalCode = c;
				} else {
					state = SS_String;
					lexemString += '\\';
					processChar( c );
				}
				break;
			case SS_StringMayOctalCode:
				if( octalCode.length() == 3 ) {
					state = SS_String;
					int charCode = ( ( octalCode[0] - '0' ) * 8 +
						( octalCode[1] - '0' ) ) * 8 + octalCode[2] - '0';
					assert( charCode % 256 != 0 );
					lexemString += static_cast<char>( charCode % 256 );
					processChar( c );
				} else {
					assert( octalCode.length() < 3 );
					if( c >= '0' && c <= '7' ) {
						octalCode += c;
					} else {
						state = SS_String;
						if( octalCode == "0" ) {
							assert( false );
							lexemString += '\0';
						} else {
							lexemString += '\\' + octalCode;
						}
						processChar( c );
					}
				}
				break;
			default:
				assert( false );
				break;
		}
	}
}

void CScanner::processEndOfFile()
{
	switch( state ) {
		case SS_BeginOfLine:
		case SS_BeginBlank:
		case SS_BeginComment:
			break;

		case SS_NotBeginOfLine:
		case SS_NotBeginBlank:
		case SS_NotBeginComment:
			offset = localOffset;
			lexem = L_NewLine;
			ProcessLexem();
			break;

		case SS_Identificator:
			state = SS_NotBeginOfLine;
			lexem = L_Identificator;
			ProcessLexem();
			processEndOfFile();
			return;
			break;

		case SS_StringAfterQuote:
			state = SS_NotBeginOfLine;
			lexem = L_String;
			ProcessLexem();
			processEndOfFile();
			return;
			break;

		case SS_String:
		case SS_StringAfterBackslash:
		case SS_StringMayOctalCode:
			error( SEC_UnclosedStringConstantAtTheEndOfFile );
			break;

		case SS_BeginPlus:
		case SS_NotBeginPlus:
		case SS_BeginCommentAfterPlus:
		case SS_NotBeginCommentAfterPlus:
			error( SEC_UnexpectedEndOfFile );
			break;

		case SS_BeginLabelOrNumber:
			error( SEC_UnclosedLabelOrNumberAtTheEndOfFile );
			break;

		case SS_NotBeginLabel:
			error( SEC_UnclosedLabelAtTheEndOfFile );
			break;

		case SS_NotBeginNumber:
			error( SEC_UnclosedNumberAtTheEndOfFile );
			break;

		case SS_BeginQualifier:
		case SS_NotBeginQualifier:
			error( SEC_UnclosedQualifierAtTheEndOfFile );
			break;

		default:
			assert( false );
			break;
	}

	lexem = L_EndOfFile;
	ProcessLexem();
}

} // end of namespace Refal2
