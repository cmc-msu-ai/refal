#include <Refal2.h>

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
	ruleParser.Reset();
	qualifierParser.Reset();
	state = S_Initial;
	storedName.clear();
	entryLabel = InvalidLabel;
	currentFunction = InvalidLabel;
}

bool CParser::wordIs( const std::string& value ) const
{
	if( token.type == TT_Word ) {
		std::string lowerWord( token.value.word );
		MakeLower( lowerWord );
		return ( lowerWord == value );
	}
	return false;
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
		addEndOfFunction(); // action
		ruleParser.EndFunction(); // action
		storedName = token.value.word; // action
		state = S_Word;
	} else if( token.type == TT_Blank ) {
		state = S_Blank;
	} else if( token.type != TT_LineFeed ) {
		addEndOfFunction(); // action
		ruleParser.EndFunction(); // action
		error( EC_LineShouldBeginWithIdentifierOrSpace );
		state = S_IgnoreLine;
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
		ruleParser.BeginFunction( storedName ); // action
		addDeclarationOfFunction( storedName ); // action
		state = S_Initial;
	} else if( token.type == TT_Blank ) {
		state = S_WordBlank;
	} else {
		addDeclarationOfFunction( storedName ); // action
		ruleParser.BeginFunction( storedName ); // action
		state = S_Rule;
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
		ruleParser.BeginFunction( storedName ); // action
		state = S_Rule;
		AddToken();
	}
}

void CParser::parsingWordBlankS()
{
	if( token.type == TT_Blank ) {
		qualifierParser.BeginNamedQualifier( storedName );
		state = S_Qualifier;
	} else {
		addDeclarationOfFunction( storedName ); // action
		CToken savedToken = token;
		token.type = TT_Word;
		token.position = storedOffset;
		token.value.word = "s";
		ruleParser.BeginFunction( storedName ); // action
		state = S_Rule;
		AddToken();
		token = savedToken;
		AddToken();
	}
}

void CParser::parsingBlank()
{
	if( token.type == TT_Word ) {
		if( wordIs( "start" )
			|| wordIs( "end" )
			|| wordIs( "entry" )
			|| wordIs( "empty" )
			|| wordIs( "swap" )
			|| wordIs( "extrn" ) )
		{
			addEndOfFunction(); // action
			state = S_Directive;
			return;
		}
	}
	ruleParser.BeginRule();
	state = S_Rule;
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
			state = S_Initial;
		}
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
