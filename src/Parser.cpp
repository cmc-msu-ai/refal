#include <iostream>
#include <Refal2.h>

namespace Refal2 {

void CParser::Reset()
{
	state = PS_Begin;
	storedName.clear();
	currentFunction = InvalidLabel;
	functionBuilder.Reset();
	namedQualifiers.clear();
	qualifierBuilder.Reset();
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
				state = PS_ProcessRuleDirection;
				ProcessLexem();
			}
			break;
		case PS_BeginIdentBlankS:
			if( lexem == L_Blank ) {
				state = PS_BeginProcessNamedQualifier;
			} else {
				addDeclarationOfFunction( storedName ); // action

				TLexem tmpLexem = lexem;
				state = PS_ProcessRuleDirection;
					
				std::string tmpLexemString( "s" );
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
				state = PS_ProcessRuleDirection;
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
		/* process named qualifier */
		case PS_BeginProcessNamedQualifier:
			qualifierBuilder.Reset();
			// TODO: check errors
			state = PS_ProcessNamedQualifier;
			ProcessLexem();
			break;
		case PS_ProcessNamedQualifier:
			processNamedQualifier();
			break;
		case PS_ProcessNamedQualifierAfterRightParen:
			state = PS_ProcessNamedQualifier;
			processNamedQualifier( true /* afterRightParen */ );
			break;
		case PS_ProcessNamedQualifierAfterError:
			processNamedQualifierAfterError();
			break;
		/* process variable qualifer */
		case PS_BeginProcessVariableQualifier:
			qualifierBuilder.Reset();
			// TODO: check errors
			state = PS_ProcessVariableQualifier;
			ProcessLexem();
			break;
		case PS_ProcessVariableQualifier:
			processVariableQualifier();
			break;
		case PS_ProcessVariableQualifierAfterRightParen:
			state = PS_ProcessVariableQualifier;
			processVariableQualifier( true /* afterRightParen */ );
			break;
		case PS_ProcessVariableQualifierAfterError:
			processVariableQualifierAfterError();
			break;
		/* process rule */
		case PS_ProcessRuleDirection:
			if( lexem == L_Blank ) {
			} else if( identificatorIs("l") ) {
				state = PS_ProcessLeftPartOfRule;
				functionBuilder.AddDirection();
			} else if( identificatorIs("r") ) {
				state = PS_ProcessLeftPartOfRule;
				functionBuilder.AddDirection( true /* isRightDirection */ );
			} else {
				state = PS_ProcessLeftPartOfRule;
				functionBuilder.AddDirection();
				ProcessLexem();
			}
			break;
		case PS_ProcessLeftPartOfRule:
			switch( lexem ) {
				case L_Blank:
					break;
				case L_Equal:
					state = PS_ProcessRightPartOfRule;
					functionBuilder.AddEndOfLeft();
					break;
				case L_Comma:
					// TODO: error, ignore
					break;
				case L_Label:
					functionBuilder.AddLabel( lexemLabel );
					break;
				case L_Number:
					functionBuilder.AddNumber( lexemNumber );
					break;
				case L_String:
					for( std::size_t i = 0; i < lexemString.size(); i++ ) {
						functionBuilder.AddChar( lexemString[i] );
					}
					break;
				case L_Qualifier:
					// TODO: error, ignore
					break;
				case L_LeftParen:
					functionBuilder.AddLeftParen();
					break;
				case L_RightParen:
					functionBuilder.AddRightParen();
					break;
				case L_LeftBracket:
					// TODO: error, ignore
					break;
				case L_RightBracket:
					// TODO: error, ignore
					break;
				case L_Identificator:
					for( std::size_t i = 0; i < lexemString.size(); i += 2 ) {
						TVariableType type = ::tolower( lexemString[i] );

						if( i < lexemString.size() - 1 ) {
							TVariableName name = lexemString[i + 1];
							functionBuilder.AddLeftVariable( type, name );
							offset += 2;
						} else {
							state = PS_ProcessLeftPartOfRuleAfterVariableType;
							variableType = type;
						}
					}
					break;
				case L_Newline:
					state = PS_Begin;
					functionBuilder.AddEndOfLeft();
					functionBuilder.AddEndOfRight();
					// TODO: error
					break;
				case L_EndOfFile:
					lexem = L_Newline;
					ProcessLexem();
					lexem = L_EndOfFile;
					ProcessLexem();
					break;
				default:
					assert( false );
					break;
			}
			break;
		case PS_ProcessLeftPartOfRuleAfterVariableType:
			if( lexem == L_LeftParen ) {
				state = PS_BeginProcessVariableQualifier;
			} else if( lexem == L_Qualifier ) {
				state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
				TQualifierMap::const_iterator qualifier = namedQualifiers.end();
				qualifier = namedQualifiers.find( ToLower( lexemString ) );
				if( qualifier != namedQualifiers.end() ) {
					currentQualifier = qualifier->second;
				} else {
					// TODO: error, ignore
				}
			} else {
				state = PS_ProcessLeftPartOfRule;
				// TODO: error, ignore
			}
			break;
		case PS_ProcessLeftPartOfRuleAfterVariableQualifier:
			state = PS_ProcessLeftPartOfRule;
			if( lexem == L_Identificator ) {
				TVariableName name = lexemString[0];
				lexemString.erase(0, 1);
				functionBuilder.AddLeftVariable( variableType, name,
					&currentQualifier );

				if( !lexemString.empty() ) {
					offset++;
					ProcessLexem();
				}
			} else {
				// TODO: error, ignore
			}
			break;
		case PS_ProcessRightPartOfRule:
			switch( lexem ) {
				case L_Blank:
					break;
				case L_Equal:
					functionBuilder.AddEndOfLeft();
					break;
				case L_Comma:
					// TODO: error, ignore
					break;
				case L_Label:
					functionBuilder.AddLabel( lexemLabel );
					break;
				case L_Number:
					functionBuilder.AddNumber( lexemNumber );
					break;
				case L_String:
					for( std::size_t i = 0; i < lexemString.size(); i++ ) {
						functionBuilder.AddChar( lexemString[i] );
					}
					break;
				case L_Qualifier:
					// TODO: error, ignore
					break;
				case L_LeftParen:
					functionBuilder.AddLeftParen();
					break;
				case L_RightParen:
					functionBuilder.AddRightParen();
					break;
				case L_LeftBracket:
					state = PS_ProcessRightPartOfRuleAfterLeftBracket;
					functionBuilder.AddLeftBracket();
					break;
				case L_RightBracket:
					functionBuilder.AddRightBracket();
					break;
				case L_Identificator:
					for( std::size_t i = 0; i < lexemString.size(); i += 2 ) {
						TVariableType type = ::tolower( lexemString[i] );

						if( i < lexemString.size() - 1 ) {
							TVariableName name = lexemString[i + 1];
							functionBuilder.AddRightVariable( type, name );
							offset += 2;
						} else {
							// TODO: error, ignore
						}
					}
					break;
				case L_Newline:
					functionBuilder.AddEndOfRight();
					state = PS_Begin;
					break;
				case L_EndOfFile:
					lexem = L_Newline;
					ProcessLexem();
					lexem = L_EndOfFile;
					ProcessLexem();
					break;
				default:
					assert( false );
					break;
			}
			break;
		case PS_ProcessRightPartOfRuleAfterLeftBracket:
			state = PS_ProcessRightPartOfRule;
			if( lexem == L_Identificator ) {
				TLabel label = LabelTable.AddLabel( ToLower( lexemString ) );
				functionBuilder.AddLabel( label );
			} else {
				ProcessLexem();
			}
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processNamedQualifier(const bool afterRightParen)
{
	switch( lexem ) {
		case L_Blank:
			break;
		case L_Equal:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: Error, ignore
			break;
		case L_Comma:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: Error, ignore
			break;
		case L_Label:
			qualifierBuilder.AddLabel( lexemLabel );
			break;
		case L_Number:
			qualifierBuilder.AddNumber( lexemNumber );
			break;
		case L_String:
			for( std::size_t i = 0; i < lexemString.size(); i++ ) {
				qualifierBuilder.AddChar( lexemString[i] );
			}
			break;
		case L_Qualifier:
		{
			TQualifierMap::const_iterator qualifier = namedQualifiers.end();
			qualifier = namedQualifiers.find( ToLower( lexemString ) );
			if( qualifier != namedQualifiers.end() ) {
				qualifierBuilder.AddQualifier( qualifier->second );
			} else {
				// TODO: error, ignore
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				state = PS_ProcessNamedQualifierAfterError;
				// TODO: error, ignore
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
				state = PS_ProcessNamedQualifierAfterRightParen;
			} else {
				state = PS_ProcessNamedQualifierAfterError;
				// TODO: error, ignore
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: error, ignore
			break;
		case L_RightBracket:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: error, ignore
			break;
		case L_Identificator:
		{
			std::string tmpLexemString = ToLower( lexemString );
			for( std::size_t i = 0; i < tmpLexemString.size(); i++ ) {
				switch( tmpLexemString[i] ) {
					case 's': qualifierBuilder.AddS(); break;
					case 'f': qualifierBuilder.AddF(); break;
					case 'n': qualifierBuilder.AddN(); break;
					case 'o': qualifierBuilder.AddO(); break;
					case 'l': qualifierBuilder.AddL(); break;
					case 'd': qualifierBuilder.AddD(); break;
					case 'w': qualifierBuilder.AddW(); break;
					case 'b': qualifierBuilder.AddB(); break;
					default:
						state = PS_ProcessNamedQualifierAfterError;
						// TODO: error, ignore
						break;
				}
			}
			break;
		}
		case L_Newline:
			if( !afterRightParen ) {
				qualifierBuilder.AddNegative();
			}
			qualifierBuilder.Export( &currentQualifier );
			addNamedQualifier();
			state = PS_Begin;
			break;
		case L_EndOfFile:
			lexem = L_Newline;
			processNamedQualifier( afterRightParen );
			lexem = L_EndOfFile;
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processNamedQualifierAfterError()
{
	switch( lexem ) {
		case L_Blank:
			break;
		case L_Equal:
			// TODO: Error, ignore
			break;
		case L_Comma:
			// TODO: Error, ignore
			break;
		case L_Label:
		case L_Number:
		case L_String:
			break;
		case L_Qualifier:
		{
			TQualifierMap::const_iterator qualifier = namedQualifiers.end();
			qualifier = namedQualifiers.find( ToLower( lexemString ) );
			if( qualifier == namedQualifiers.end() ) {
				// TODO: error, ignore
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				// TODO: error, ignore
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
			} else {
				// TODO: error, ignore
			}
			break;
		case L_LeftBracket:
			// TODO: error, ignore
			break;
		case L_RightBracket:
			// TODO: error, ignore
			break;
		case L_Identificator:
		{
			std::string tmpLexemString = ToLower( lexemString );
			for( std::size_t i = 0; i < tmpLexemString.size(); i++ ) {
				switch( tmpLexemString[i] ) {
					case 's': case 'f': case 'n': case 'o':
					case 'l': case 'd': case 'w': case 'b':
						break;
					default:
						// TODO: error, ignore
						break;
				}
			}
			break;
		}
		case L_Newline:
			state = PS_Begin;
			break;
		case L_EndOfFile:
			state = PS_Begin;
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processVariableQualifier(const bool afterRightParen)
{
	switch( lexem ) {
		case L_Blank:
			break;
		case L_Equal:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_Comma:
			state = PS_ProcessVariableQualifierAfterError;
			// TODO: Error, ignore
			break;
		case L_Label:
			qualifierBuilder.AddLabel( lexemLabel );
			break;
		case L_Number:
			qualifierBuilder.AddNumber( lexemNumber );
			break;
		case L_String:
			for( std::size_t i = 0; i < lexemString.size(); i++ ) {
				qualifierBuilder.AddChar( lexemString[i] );
			}
			break;
		case L_Qualifier:
		{
			TQualifierMap::const_iterator qualifier = namedQualifiers.end();
			qualifier = namedQualifiers.find( ToLower( lexemString ) );
			if( qualifier != namedQualifiers.end() ) {
				qualifierBuilder.AddQualifier( qualifier->second );
			} else {
				// TODO: error, ignore
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				state = PS_ProcessVariableQualifierAfterError;
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
				state = PS_ProcessVariableQualifierAfterRightParen;
			} else {
				state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
				qualifierBuilder.Export( &currentQualifier );
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_RightBracket:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_Identificator:
		{
			std::string tmpLexemString = ToLower( lexemString );
			for( std::size_t i = 0; i < tmpLexemString.size(); i++ ) {
				switch( tmpLexemString[i] ) {
					case 's': qualifierBuilder.AddS(); break;
					case 'f': qualifierBuilder.AddF(); break;
					case 'n': qualifierBuilder.AddN(); break;
					case 'o': qualifierBuilder.AddO(); break;
					case 'l': qualifierBuilder.AddL(); break;
					case 'd': qualifierBuilder.AddD(); break;
					case 'w': qualifierBuilder.AddW(); break;
					case 'b': qualifierBuilder.AddB(); break;
					default:
						state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
						// TODO: error
						ProcessLexem();
						return;
						break;
				}
			}
			break;
		}
		case L_Newline:
		case L_EndOfFile:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::processVariableQualifierAfterError()
{
	switch( lexem ) {
		case L_Blank:
			break;
		case L_Equal:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_Comma:
			// TODO: Error, ignore
			break;
		case L_Label:
		case L_Number:
		case L_String:
			break;
		case L_Qualifier:
		{
			TQualifierMap::const_iterator qualifier = namedQualifiers.end();
			qualifier = namedQualifiers.find( ToLower( lexemString ) );
			if( qualifier == namedQualifiers.end() ) {
				// TODO: error, ignore
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				// TODO: error, ignore
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
			} else {
				state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_RightBracket:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		case L_Identificator:
		{
			std::string tmpLexemString = ToLower( lexemString );
			for( std::size_t i = 0; i < tmpLexemString.size(); i++ ) {
				switch( tmpLexemString[i] ) {
					case 's': case 'f': case 'n': case 'o':
					case 'l': case 'd': case 'w': case 'b':
						break;
					default:
						state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
						// TODO: error
						ProcessLexem();
						return;
						break;
				}
			}
			break;
		}
		case L_Newline:
		case L_EndOfFile:
			state = PS_ProcessLeftPartOfRuleAfterVariableQualifier;
			// TODO: error
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::addDeclarationOfFunction(const std::string& name)
{
	addEndOfFunction();

	currentFunction = LabelTable.AddLabel( name );
	CFunction* tmpFunction = LabelTable.GetLabelFunction( currentFunction );

	if( !tmpFunction->IsDeclared() ) {
		currentFunction = InvalidLabel;
		// TODO: error, ignore
	} else {
		std::cout << "addDeclarationOfFunction: {" << name << "}\n";
		tmpFunction->SetDefined();
	}
}

void CParser::addEndOfFunction()
{
	if( currentFunction != InvalidLabel ) {
		CFunction* tmpFunction = LabelTable.GetLabelFunction( currentFunction );
		functionBuilder.Export( tmpFunction );
		PrintFunction( tmpFunction->firstRule );

		std::cout << "addEndOfFunction: {" <<
			LabelTable.GetLabelText( currentFunction ) << "}\n";

		currentFunction = InvalidLabel;
	}
	functionBuilder.Reset();
}

void CParser::addNamedQualifier()
{
	typedef std::pair<TQualifierMap::iterator, bool> TPair;
	
	TPair pair = namedQualifiers.insert(
		std::make_pair( storedName, currentQualifier ) );
	
	if( pair.second ) {
		// TODO: error, redeclaration of qualifier
	}
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
