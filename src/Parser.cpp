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
	entryLabel = InvalidLabel;
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
				addDeclarationOfFunction( storedName ); // action
			} else if( lexem == L_Blank ) {
				state = PS_BeginIdentBlank;
			} else {
				state = PS_ProcessRule;
				addDeclarationOfFunction( storedName ); // action
				ProcessLexem();
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
				/* TODO: error */ error( PEC_STUB );
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
			} else if( identificatorIs( "start" ) ) {
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
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		case PS_BeginEntryBlank:
			if( lexem == L_Identificator ) {
				state = PS_OnlyNewline;
				addEntryFunction(ToLower(lexemString) ); // action
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		/* end of entry */
		/* empty */
		case PS_BeginEmpty:
			if( lexem == L_Blank ) {
				state = PS_BeginEmptyComma;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		case PS_BeginEmptyComma:
			if( lexem == L_Identificator ) {
				state = PS_BeginEmptyIdent;
				addEmptyFunction( ToLower(lexemString) ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
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
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		/* end of empty */
		/* extrn */
		case PS_BeginExtrn:
			if( lexem == L_Blank ) {
				state = PS_BeginExtrnComma;
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		case PS_BeginExtrnComma:
			if( lexem == L_Identificator ) {
				state = PS_BeginExtrnIdent;
				storedName = ToLower( lexemString ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
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
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		case PS_BeginExtrnLeftParen:
			if( lexem == L_Identificator ) {
				state = PS_BeginExtrnInnerIdent;
				addExtrnFunction( storedName, ToLower( lexemString ) ); // action
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		case PS_BeginExtrnInnerIdent:
			if( lexem == L_RightParen ) {
				state = PS_BeginExtrnOnlyComma;
			} else if( lexem == L_Blank ) {
			} else {
				state = PS_WaitNewline;
				/* TODO: ERROR */ error( PEC_STUB );
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
				/* TODO: ERROR */ error( PEC_STUB );
			}
			break;
		/* end of extrn */
		/* process named qualifier */
		case PS_BeginProcessNamedQualifier:
			qualifierBuilder.Reset();
			if( HasErrors() ) {
				state = PS_ProcessNamedQualifierAfterError;
			} else {
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
			qualifierBuilder.Reset();
			if( HasErrors() ) {
				state = PS_ProcessVariableQualifierAfterError;
			} else {
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
			} else if( identificatorIs( "l" ) ) {
				state = PS_ProcessRule;
			} else if( identificatorIs( "r" ) ) {
				state = PS_ProcessRule;
				CFunctionBuilder::SetRightDirection();
			} else {
				state = PS_ProcessRule;
				ProcessLexem();
			}
			break;
		case PS_ProcessRule:
			switch( lexem ) {
				case L_Blank:
					break;
				case L_Equal:
					CFunctionBuilder::AddEndOfLeft();
					break;
				case L_Comma:
					// TODO: error, ignore
					error( PEC_STUB );
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
					error( PEC_STUB );
					break;
				case L_LeftParen:
					CFunctionBuilder::AddLeftParen();
					break;
				case L_RightParen:
					CFunctionBuilder::AddRightParen();
					break;
				case L_LeftBracket:
					state = PS_ProcessRuleAfterLeftBracket;
					CFunctionBuilder::AddLeftBracket();
					break;
				case L_RightBracket:
					CFunctionBuilder::AddRightBracket();
					break;
				case L_Identificator:
					if( identificatorIs( "k" ) ) {
						// TODO: warning: old style
						state = PS_ProcessRuleAfterLeftBracket;
						CFunctionBuilder::AddLeftBracket();
						break;
					}
					for( std::size_t i = 0; i < lexemString.length(); i += 2 ) {
						TVariableType type = ::tolower( lexemString[i] );
						if( i < lexemString.length() - 1 ) {
							TVariableName name = lexemString[i + 1];
							CFunctionBuilder::AddVariable( type, name );
							offset += 2;
						} else {
							state = PS_ProcessRuleAfterVariableType;
							variableType = type;
						}
					}
					break;
				case L_NewLine:
					state = PS_Begin;
					CFunctionBuilder::AddEndOfRight();
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
		case PS_ProcessRuleAfterVariableType:
			if( lexem == L_LeftParen ) {
				state = PS_BeginProcessVariableQualifier;
			} else if( lexem == L_Qualifier ) {
				state = PS_ProcessRuleAfterVariableQualifier;
				CQualifierMap::const_iterator qualifier = namedQualifiers.end();
				qualifier = namedQualifiers.find( ToLower( lexemString ) );
				if( qualifier != namedQualifiers.end() ) {
					currentQualifier = qualifier->second;
				} else {
					// TODO: error, ignore
					error( PEC_STUB );
				}
			} else {
				state = PS_ProcessRule;
				// TODO: error, ignore
				error( PEC_STUB );
			}
			break;
		case PS_ProcessRuleAfterVariableQualifier:
			state = PS_ProcessRule;
			if( lexem == L_Identificator ) {
				TVariableName name = lexemString[0];
				lexemString.erase(0, 1);
				CFunctionBuilder::AddVariable( variableType, name,
					&currentQualifier );

				if( !lexemString.empty() ) {
					offset++;
					ProcessLexem();
				}
			} else {
				// TODO: error, ignore
				error( PEC_STUB );
			}
			break;
		case PS_ProcessRuleAfterLeftBracket:
			state = PS_ProcessRule;
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
			error( PEC_STUB );
			break;
		case L_Comma:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: Error, ignore
			error( PEC_STUB );
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
				error( PEC_STUB );
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				state = PS_ProcessNamedQualifierAfterError;
				// TODO: error, ignore
				error( PEC_STUB );
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
				error( PEC_STUB );
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: error, ignore
			error( PEC_STUB );
			break;
		case L_RightBracket:
			state = PS_ProcessNamedQualifierAfterError;
			// TODO: error, ignore
			error( PEC_STUB );
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
						error( PEC_STUB );
						break;
				}
			}
			break;
		}
		case L_NewLine:
			if( !afterRightParen ) {
				qualifierBuilder.AddNegative();
			}
			qualifierBuilder.Export( currentQualifier );
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
			error( PEC_STUB );
			break;
		case L_Comma:
			// TODO: Error, ignore
			error( PEC_STUB );
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
				error( PEC_STUB );
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				// TODO: error, ignore
				error( PEC_STUB );
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
			} else {
				// TODO: error, ignore
				error( PEC_STUB );
			}
			break;
		case L_LeftBracket:
			// TODO: error, ignore
			error( PEC_STUB );
			break;
		case L_RightBracket:
			// TODO: error, ignore
			error( PEC_STUB );
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
						error( PEC_STUB );
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
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
			ProcessLexem();
			break;
		case L_Comma:
			state = PS_ProcessVariableQualifierAfterError;
			// TODO: Error, ignore
			error( PEC_STUB );
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
				error( PEC_STUB );
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
				state = PS_ProcessRuleAfterVariableQualifier;
				if( !afterRightParen ) {
					qualifierBuilder.AddNegative();
				}
				qualifierBuilder.Export( currentQualifier );
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
			ProcessLexem();
			break;
		case L_RightBracket:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
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
						state = PS_ProcessRuleAfterVariableQualifier;
						// TODO: error
						error( PEC_STUB );
						ProcessLexem();
						return;
						break;
				}
			}
			break;
		}
		case L_NewLine:
		case L_EndOfFile:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
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
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
			ProcessLexem();
			break;
		case L_Comma:
			// TODO: Error, ignore
			error( PEC_STUB );
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
				error( PEC_STUB );
			}
			break;
		}
		case L_LeftParen:
			if( qualifierBuilder.IsNegative() ) {
				// TODO: error, ignore
				error( PEC_STUB );
			} else {
				qualifierBuilder.AddNegative();
			}
			break;
		case L_RightParen:
			if( qualifierBuilder.IsNegative() ) {
				qualifierBuilder.AddNegative();
			} else {
				state = PS_ProcessRuleAfterVariableQualifier;
			}
			break;
		case L_LeftBracket:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
			ProcessLexem();
			break;
		case L_RightBracket:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
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
						state = PS_ProcessRuleAfterVariableQualifier;
						// TODO: error
						error( PEC_STUB );
						ProcessLexem();
						return;
						break;
				}
			}
			break;
		}
		case L_NewLine:
		case L_EndOfFile:
			state = PS_ProcessRuleAfterVariableQualifier;
			// TODO: error
			error( PEC_STUB );
			ProcessLexem();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::addDeclarationOfFunction( const std::string& name )
{
	addEndOfFunction();

	currentFunction = LabelTable.AddLabel( name );
	CFunction& tmpFunction = LabelTable.GetLabelFunction( currentFunction );

	if( !tmpFunction.IsDeclared() ) {
		// TODO: error, ignore
		error( PEC_STUB );
	} else {
		tmpFunction.SetDefined();
		std::cout << "addDeclarationOfFunction: {" << name << "}\n";
	}
}

void CParser::addEndOfFunction()
{
	if( currentFunction != InvalidLabel ) {
		CFunction& tmpFunction = LabelTable.GetLabelFunction( currentFunction );
		if( tmpFunction.IsDefined() ) {
			CFunctionBuilder::Export( tmpFunction );
			if( tmpFunction.IsParsed() ) {
				PrintFunction( tmpFunction );
			}
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
	
	if( !pair.second ) {
		// TODO: error, redeclaration of qualifier
		error( PEC_STUB );
	}
}

void CParser::addEmptyFunction( const std::string& name )
{
	std::cout << "Empty: " << name << "\n";
	TLabel emptyLabel = LabelTable.AddLabel( name );
	CFunction& emptyFunction = LabelTable.GetLabelFunction( emptyLabel );
	if( emptyFunction.IsDeclared() ) {
		emptyFunction.SetEmpty();
	} else if( emptyFunction.IsEmpty() ) {
		// TODO: warning, such empty function %function% already defined
	} else if( emptyFunction.IsDefined() || emptyFunction.IsParsed() ) {
		// TODO: error, %function% already defined in program
		error( PEC_STUB );
	} else if( emptyFunction.IsExternal() ) {
		// TODO: error, %function% already defined as external function
		error( PEC_STUB );
	} else {
		assert( false );
	}
}

void CParser::addEntryFunction( const std::string& name )
{
	std::cout << "Entry: " << name << "\n";
	if( entryLabel == InvalidLabel ) {
		entryLabel = LabelTable.AddLabel( name );
		CFunction& entryFunction = LabelTable.GetLabelFunction( entryLabel );
		if( entryFunction.IsEmpty() ) {
			// TODO: error, entry %function% cannot be empty function
			error( PEC_STUB );
		} else if( entryFunction.IsExternal() ) {
			// TODO: error, entry %function% cannot be external function
			error( PEC_STUB );
		}
	} else {
		// TODO: error, redeclaration of entry
		error( PEC_STUB );
	}
}

bool ExtrnPrint( CUnitList& argument, std::string& errorText );
bool ExtrnPrintm( CUnitList& argument, std::string& errorText );
bool ExtrnProut( CUnitList& argument, std::string& errorText );
bool ExtrnProutm( CUnitList& argument, std::string& errorText );

void CParser::addExtrnFunction( const std::string& name,
	const std::string& standartName )
{
	std::cout << "Extrn: " << name << "(" << standartName << ")\n";
	TLabel externalLabel = LabelTable.AddLabel( name );
	CFunction& externalFunction = LabelTable.GetLabelFunction( externalLabel );
	if( externalFunction.IsDeclared() ) {
		// TODO: add external
		if( standartName == "print" ) {
			externalFunction.SetExternal( ExtrnPrint );
		} else if( standartName == "printm" ) {
			externalFunction.SetExternal( ExtrnPrintm );
		} else if( standartName == "prout" ) {
			externalFunction.SetExternal( ExtrnProut );
		} else if( standartName == "proutm" ) {
			externalFunction.SetExternal( ExtrnProutm );
		} else {
			//error( PEC_STUB );
		}
	} else if( externalFunction.IsEmpty() ) {
		// TODO: error, %function% already defined as empty function
		error( PEC_STUB );
	} else if( externalFunction.IsDefined() || externalFunction.IsParsed() ) {
		// TODO: error, %function% already defined in program
		error( PEC_STUB );
	} else if( externalFunction.IsExternal() ) {
		// TODO: error, %function% already defined as external function
		error( PEC_STUB );
	} else {
		assert( false );
	}
}

} // end of namespace Refal2
