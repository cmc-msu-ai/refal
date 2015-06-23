#include <Refal2.h>
#include <iostream>
#include <sstream>

namespace Refal2 {

CParser::CParser( IErrorHandler* errorProcessor ):
	CErrorsHelper( errorProcessor ),
	ruleParser( errorProcessor ),
	qualifierParser( errorProcessor ),
	functionBuilder( errorProcessor )
{
	Reset();
}

void CParser::Reset()
{
	state = S_Initial;
	storedName.clear();
	entryLabel = InvalidLabel;
	currentFunction = InvalidLabel;
	namedQualifiers.clear();
}

static std::string ToLower(const std::string& data)
{
	std::string result( data );
	std::transform( result.begin(), result.end(), result.begin(), ::tolower );
	return result;
}

bool CParser::wordIs( const std::string& value ) const
{
	return ( token.type == TT_Word && ToLower( token.value.word ) == value );
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
		case S_Directive:
			parsingDirective();
			break;
		case S_Qualifier:
			parsingQualifier();
			break;
		case S_RuleDirection:
			parsingRuleDirection();
			break;
		case S_Rule:
			parsingRule();
			break;
		default:
			assert( false );
			break;
	}
}

void CParser::parsingInitial()
{
	if( token.type == TT_Word ) {
		state = S_Word;
		storedName = ToLower( token.value.word ); // action
		addEndOfFunction(); // action
	} else if( token.type == TT_Blank ) {
		state = S_Blank;
	} else if( token.type != TT_LineFeed ) {
		state = S_IgnoreLine;
		addEndOfFunction(); // action
		error( EC_LineShouldBeginWithIdentifierOrSpace );
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
		state = S_Initial;
		addDeclarationOfFunction( storedName ); // action
	} else if( token.type == TT_Blank ) {
		state = S_WordBlank;
	} else {
		state = S_RuleDirection;
		addDeclarationOfFunction( storedName ); // action
		AddToken();
	}
}

void CParser::parsingWordBlank()
{
	if( wordIs( "start" ) ) {
		state = S_Directive;
		/* TODO: WARNING */
	} else if( wordIs( "s" ) ) {
		state = S_WordBlankS;
		storedOffset = token.position;
	} else {
		addDeclarationOfFunction( storedName ); // action
		state = S_RuleDirection;
		AddToken();
	}
}

void CParser::parsingWordBlankS()
{
	if( token.type == TT_Blank ) {
		qualifierParser.Reset( true /* isNamed */ );
		state = S_Qualifier;
	} else {
		addDeclarationOfFunction( storedName ); // action
		CToken savedToken = token;
		token.type = TT_Word;
		token.position = storedOffset;
		token.value.word = "s";
		state = S_RuleDirection;
		AddToken();
		token = savedToken;
		AddToken();
	}
}

void CParser::parsingBlank()
{
	if( token.type == TT_Word ) {
		std::string directiveName = ToLower( token.value.word );
		if( directiveName == "start"
			|| directiveName == "end"
			|| directiveName == "entry"
			|| directiveName == "empty"
			|| directiveName == "swap"
			|| directiveName == "extrn" )
		{
			addEndOfFunction(); // action
			state = S_Directive;
			return;
		}
	}
	state = S_RuleDirection;
	AddToken();
}

void CParser::parsingDirective()
{
	std::ostringstream errorStream;
	errorStream << ":" << token.line << ":" << token.position
		<< ": error in directive: " << "not implemented yet" << ".";
	CErrorsHelper::Error( errorStream.str() );
	state = S_IgnoreLine;
}

void CParser::parsingQualifier()
{
	if( qualifierParser.AddToken( token ) ) {
		if( qualifierParser.HasErrors() ) {
			state = S_IgnoreLine;
			AddToken();
		} else {
			// TODO: save qualifier
			if( qualifierParser.IsNamed() ) {
				state = S_Initial;
			} else {
				state = S_Rule;
			}
		}
	}
}

void CParser::parsingRuleDirection()
{
	if( wordIs( "l" ) ) {
		ruleParser.Reset();
		state = S_Rule;
	} else if( wordIs( "r" ) ) {
		ruleParser.Reset();
		state = S_Rule;
		functionBuilder.SetRightDirection();
	} else if( token.type != TT_Blank ) {
		ruleParser.Reset();
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingRule()
{
	if( ruleParser.AddToken( token ) ) {
		if( ruleParser.HasErrors() ) {
			state = S_IgnoreLine;
			AddToken();
		} else {
			state = S_Initial;
		}
	}
#if 0
				switch( token.type ) {
				case TT_Blank:
					break;
				case TT_Equal:
					functionBuilder.AddEndOfLeft();
					break;
				case TT_Comma:
					// TODO: error, ignore
					error( EC_STUB );
					break;
				case TT_Label:
					functionBuilder.AddLabel( LabelTable.AddLabel( ToLower( token.value.word ) ) );
					break;
				case TT_Number:
					functionBuilder.AddNumber( token.value.number );
					break;
				case TT_String:
					for( std::size_t i = 0; i < token.value.string.size(); i++ ) {
						functionBuilder.AddChar( token.value.string[i] );
					}
					break;
				case TT_Qualifier:
					// TODO: error, ignore
					error( EC_STUB );
					break;
				case TT_LeftParen:
					functionBuilder.AddLeftParen();
					break;
				case TT_RightParen:
					functionBuilder.AddRightParen();
					break;
				case TT_LeftBracket:
					state = PS_ProcessRuleAfterLeftBracket;
					functionBuilder.AddLeftBracket();
					break;
				case TT_RightBracket:
					functionBuilder.AddRightBracket();
					break;
				case TT_Word:
					if( IdentificatorIs( token, "k" ) ) {
						// TODO: warning: old style
						state = PS_ProcessRuleAfterLeftBracket;
						functionBuilder.AddLeftBracket();
						break;
					}
					for( std::size_t i = 0; i < token.value.word.length(); i += 2 ) {
						TVariableType type = ::tolower( token.value.word[i] );
						if( i < token.value.word.length() - 1 ) {
							TVariableName name = token.value.word[i + 1];
							functionBuilder.AddVariable( type, name );
							token.position += 2;
						} else {
							state = PS_ProcessRuleAfterVariableType;
							variableType = type;
						}
					}
					break;
				case TT_LineFeed:
					state = PS_Begin;
					functionBuilder.AddEndOfRight();
					break;
				/*case L_EndOfFile:
					token.type = TT_LineFeed;
					ProcessLexem();
					token.type = L_EndOfFile;
					ProcessLexem();
					break;*/
				default:
					assert( false );
					break;
			}
			break;
		case PS_ProcessRuleAfterVariableType:
			if( token.type == TT_LeftParen ) {
				state = PS_BeginProcessVariableQualifier;
			} else if( token.type == TT_Qualifier ) {
				state = PS_ProcessRuleAfterVariableQualifier;
				CQualifierMap::const_iterator qualifier = namedQualifiers.end();
				qualifier = namedQualifiers.find( ToLower( token.value.word ) );
				if( qualifier != namedQualifiers.end() ) {
					currentQualifier = qualifier->second;
				} else {
					// TODO: error, ignore
					error( EC_STUB );
				}
			} else {
				state = PS_ProcessRule;
				// TODO: error, ignore
				error( EC_STUB );
			}
			break;
		case PS_ProcessRuleAfterVariableQualifier:
			state = PS_ProcessRule;
			if( token.type == TT_Word ) {
				TVariableName name = token.value.word[0];
				token.value.word.erase(0, 1);
				functionBuilder.AddVariable( variableType, name,
					&currentQualifier );

				if( !token.value.word.empty() ) {
					token.position++;
					AddToken();
				}
			} else {
				// TODO: error, ignore
				error( EC_STUB );
			}
			break;
		case PS_ProcessRuleAfterLeftBracket:
			state = PS_ProcessRule;
			if( token.type == TT_Word ) {
				TLabel label = LabelTable.AddLabel( ToLower( token.value.word ) );
				functionBuilder.AddLabel( label );
			} else {
				AddToken();
			}
			break;
#endif
}

void CParser::error( TErrorCode errorCode )
{
	std::ostringstream errorStream;
	errorStream << token.line << ":" << token.position << ": error: ";
	switch( errorCode ) {
		case EC_LineShouldBeginWithIdentifierOrSpace:
			errorStream << "line should begin with identifier or space";
			break;
		case EC_NewLineExpected:
			errorStream << "line feed expected";
			break;
		case EC_UnexpectedLexemeAfterIdentifierInTheBeginningOfLine:
			errorStream << "unexpected token after identifier in the beginning of the line";
		case EC_STUB:
			errorStream << "stub" ;
			break;
		default:
			assert( false );
			break;
	}
	errorStream << ".";
	CErrorsHelper::Error( errorStream.str() );
}


void CParser::addDeclarationOfFunction( const std::string& name )
{
	addEndOfFunction();

	currentFunction = LabelTable.AddLabel( name );
	CFunction& tmpFunction = LabelTable.GetLabelFunction( currentFunction );

	if( !tmpFunction.IsDeclared() ) {
		// TODO: error, ignore
		error( EC_STUB );
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
			functionBuilder.Export( tmpFunction );
			if( tmpFunction.IsParsed() ) {
				PrintFunction( tmpFunction );
			}
			std::cout << "addEndOfFunction: {" <<
				LabelTable.GetLabelText( currentFunction ) << "}\n";
		}
		currentFunction = InvalidLabel;
		functionBuilder.Reset();
	}
}

void CParser::addNamedQualifier()
{
	typedef std::pair<CQualifierMap::iterator, bool> CPair;
	
	CPair pair = namedQualifiers.insert(
		std::make_pair( storedName, currentQualifier ) );
	
	if( !pair.second ) {
		// TODO: error, redeclaration of qualifier
		error( EC_STUB );
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
		error( EC_STUB );
	} else if( emptyFunction.IsExternal() ) {
		// TODO: error, %function% already defined as external function
		error( EC_STUB );
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
			error( EC_STUB );
		} else if( entryFunction.IsExternal() ) {
			// TODO: error, entry %function% cannot be external function
			error( EC_STUB );
		}
	} else {
		// TODO: error, redeclaration of entry
		error( EC_STUB );
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
			//error( EC_STUB );
		}
	} else if( externalFunction.IsEmpty() ) {
		// TODO: error, %function% already defined as empty function
		error( EC_STUB );
	} else if( externalFunction.IsDefined() || externalFunction.IsParsed() ) {
		// TODO: error, %function% already defined in program
		error( EC_STUB );
	} else if( externalFunction.IsExternal() ) {
		// TODO: error, %function% already defined as external function
		error( EC_STUB );
	} else {
		assert( false );
	}
}

} // end of namespace Refal2
