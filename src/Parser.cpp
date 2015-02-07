#include <iostream>
#include <Refal2.h>

namespace Refal2 {

CParser::CParser(IParserListener* listener):
	CScanner( dynamic_cast<IScannerListener*>( listener ) ),
	CListener<IParserListener>( listener )
{
	Reset();
}

void CParser::Reset()
{
	CScanner::Reset();
	state = PS_Begin;
	storedName.clear();
	currentFunction = InvalidLabel;
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
			} else if( lexem == L_EndOfFile ) {
				addEndOfFunction(); // action
			} else {
				state = PS_WaitNewline;
				addEndOfFunction(); // action
				error( PEC_LineShouldBeginWithIdentifierOrSpace );
			}
			break;
		case PS_OnlyNewline:
			if( lexem == L_NewLine ) {
				state = PS_Begin;
			} else if( lexem == L_EndOfFile ) {
				addEndOfFunction(); // action
			} else {
				state = PS_WaitNewline;
				error( PEC_NewLineExpected );
			}
			break;
		case PS_WaitNewline:
			if( lexem == L_NewLine ) {
				state = PS_Begin;
			} else if( lexem == L_EndOfFile ) {
				state = PS_Begin;
				addEndOfFunction(); // action
			}
			break;
		case PS_BeginIdent:
			if( lexem == L_NewLine || lexem == L_EndOfFile ) {
				state = PS_Begin;
				addEndOfFunction(); // action
				addDeclarationOfFunction( storedName ); // action
			} else if( lexem == L_Blank ) {
				state = PS_BeginIdentBlank;
			} else {
				error( PEC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine );
			}
			break;
		case PS_BeginIdentBlank:
			if( identificatorIs( "start" ) ) {
				state = PS_OnlyNewline;
				/* TODO: WARNING */
			} else if( identificatorIs( "s" ) ) {
				state = PS_BeginIdentBlankS;
				storedOffset = offset;
			} else if( lexem == L_EndOfFile ) {
				/* TODO: error */
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
			if( lexem == L_NewLine ) {
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
			if( lexem == L_NewLine ) {
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
			} else if( lexem == L_NewLine ) {
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
			if( HasErrors() ) {
				state = PS_ProcessNamedQualifierAfterError;
			} else {
				qualifierBuilder.Reset();
				state = PS_ProcessNamedQualifier;
			}
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
			if( HasErrors() ) {
				state = PS_ProcessVariableQualifierAfterError;
			} else {
				qualifierBuilder.Reset();
				state = PS_ProcessVariableQualifier;
			}
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
				CFunctionBuilder::AddDirection();
			} else if( identificatorIs("r") ) {
				state = PS_ProcessLeftPartOfRule;
				CFunctionBuilder::AddDirection( true /* isRightDirection */ );
			} else {
				state = PS_ProcessLeftPartOfRule;
				CFunctionBuilder::AddDirection();
				ProcessLexem();
			}
			break;
		case PS_ProcessLeftPartOfRule:
			switch( lexem ) {
				case L_Blank:
					break;
				case L_Equal:
					state = PS_ProcessRightPartOfRule;
					CFunctionBuilder::AddEndOfLeft();
					break;
				case L_Comma:
					// TODO: error, ignore
					break;
				case L_Label:
					CFunctionBuilder::AddLabel( lexemLabel );
					break;
				case L_Number:
					CFunctionBuilder::AddNumber( lexemNumber );
					break;
				case L_String:
					for( std::size_t i = 0; i < lexemString.size(); i++ ) {
						CFunctionBuilder::AddChar( lexemString[i] );
					}
					break;
				case L_Qualifier:
					// TODO: error, ignore
					break;
				case L_LeftParen:
					CFunctionBuilder::AddLeftParen();
					break;
				case L_RightParen:
					CFunctionBuilder::AddRightParen();
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
							CFunctionBuilder::AddLeftVariable( type, name );
							offset += 2;
						} else {
							state = PS_ProcessLeftPartOfRuleAfterVariableType;
							variableType = type;
						}
					}
					break;
				case L_NewLine:
					state = PS_Begin;
					CFunctionBuilder::AddEndOfLeft();
					CFunctionBuilder::AddEndOfRight();
					// TODO: error
					break;
				case L_EndOfFile:
					lexem = L_NewLine;
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
				CQualifierMap::const_iterator qualifier = namedQualifiers.end();
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
				CFunctionBuilder::AddLeftVariable( variableType, name,
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
					CFunctionBuilder::AddEndOfLeft();
					break;
				case L_Comma:
					// TODO: error, ignore
					break;
				case L_Label:
					CFunctionBuilder::AddLabel( lexemLabel );
					break;
				case L_Number:
					CFunctionBuilder::AddNumber( lexemNumber );
					break;
				case L_String:
					for( std::size_t i = 0; i < lexemString.size(); i++ ) {
						CFunctionBuilder::AddChar( lexemString[i] );
					}
					break;
				case L_Qualifier:
					// TODO: error, ignore
					break;
				case L_LeftParen:
					CFunctionBuilder::AddLeftParen();
					break;
				case L_RightParen:
					CFunctionBuilder::AddRightParen();
					break;
				case L_LeftBracket:
					state = PS_ProcessRightPartOfRuleAfterLeftBracket;
					CFunctionBuilder::AddLeftBracket();
					break;
				case L_RightBracket:
					CFunctionBuilder::AddRightBracket();
					break;
				case L_Identificator:
					for( std::size_t i = 0; i < lexemString.size(); i += 2 ) {
						TVariableType type = ::tolower( lexemString[i] );
						
						if( i < lexemString.size() - 1 ) {
							TVariableName name = lexemString[i + 1];
							CFunctionBuilder::AddRightVariable( type, name );
							offset += 2;
						} else {
							// TODO: error, ignore
						}
					}
					break;
				case L_NewLine:
					CFunctionBuilder::AddEndOfRight();
					state = PS_Begin;
					break;
				case L_EndOfFile:
					lexem = L_NewLine;
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
				CFunctionBuilder::AddLabel( label );
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
			CQualifierMap::const_iterator qualifier = namedQualifiers.end();
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
		case L_NewLine:
			if( !afterRightParen ) {
				qualifierBuilder.AddNegative();
			}
			qualifierBuilder.Export( &currentQualifier );
			addNamedQualifier();
			state = PS_Begin;
			break;
		case L_EndOfFile:
			lexem = L_NewLine;
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
			CQualifierMap::const_iterator qualifier = namedQualifiers.end();
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
		case L_NewLine:
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
			CQualifierMap::const_iterator qualifier = namedQualifiers.end();
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
				if( !afterRightParen ) {
					qualifierBuilder.AddNegative();
				}
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
		case L_NewLine:
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
			CQualifierMap::const_iterator qualifier = namedQualifiers.end();
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
		case L_NewLine:
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
		// TODO: error, ignore
	} else {
		tmpFunction->SetDefined();

		std::cout << "addDeclarationOfFunction: {" << name << "}\n";
	}
}

void CParser::addEndOfFunction()
{
	if( currentFunction != InvalidLabel ) {
		CFunction* tmpFunction = LabelTable.GetLabelFunction( currentFunction );
		
		if( !tmpFunction->IsDeclared() ) {
			CFunctionBuilder::Export( tmpFunction );
			
			PrintFunction( *tmpFunction );
			std::cout << "addEndOfFunction: {" <<
				LabelTable.GetLabelText( currentFunction ) << "}\n";
		}
		
		currentFunction = InvalidLabel;
		CFunctionBuilder::Reset();
	}
}

void CParser::addNamedQualifier()
{
	typedef std::pair<CQualifierMap::iterator, bool> CPair;
	
	CPair pair = namedQualifiers.insert(
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
