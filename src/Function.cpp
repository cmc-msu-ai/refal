#include <stack>
#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

void PrintRule( const CFunctionRule& rule )
{
	printf( "\t" );
	if( rule.isRightDirection ) {
		printf( "R" );
	}
	printf( " " );
	PrintUnitList( rule.leftPart, &rule.variables );
	printf( "= " );
	PrintUnitList( rule.rightPart );
}

void PrintFunction( const CFunction& function )
{
	assert( function.IsParsed() );
	for( const CFunctionRule* rule = function.firstRule; rule != 0;
		rule = rule->nextRule )
	{
		PrintRule( *rule );
		printf( "\n" );
	}
}

//-----------------------------------------------------------------------------

CFunctionBuilder::CFunctionBuilder( IErrorHandler* errorProcessor ):
	CVariablesBuilder( errorProcessor ),
	isProcessRightPart( false ),
	isRightDirection( false ),
	firstRule( 0 ),
	lastRule( 0 )
{
}

CFunctionBuilder::~CFunctionBuilder()
{
	Reset();
}

void CFunctionBuilder::Reset()
{
	CVariablesBuilder::Reset();
	isProcessRightPart = false;
	isRightDirection = false;
	acc.Empty();
	leftPart.Empty();
	emptyRules();
	emptyStack();
}

void CFunctionBuilder::Export( CFunction& function )
{
	// todo: check rule
	if( !HasErrors() ) {
		if( firstRule != 0 ) {
			function.SetParsed( &firstRule );
		} else {
			error( EC_ThereAreNoRulesInFunction );
		}
	}
	Reset();
}

void CFunctionBuilder::AddEndOfLeft()
{
	if( isProcessRightPart ) {
		error( EC_ThereAreMultiplePartsSeparatorInRules );
	} else {
		while( !balanceStack.empty() ) {
			balanceStack.pop();
			error( EC_UnclosedLeftParenInLeftPart );
		}
		if( HasErrors() ) {
			acc.Empty();
		} else {
			acc.Move( &leftPart );
		}
		isProcessRightPart = true;
	}
}

void CFunctionBuilder::AddEndOfRight()
{
	while( !balanceStack.empty() ) {
		CUnitNode* unit = balanceStack.top();
		balanceStack.pop();
		error( unit->IsLeftParen() ? EC_UnclosedLeftParenInRightPart :
			EC_UnclosedLeftBracketInRightPart );
	}
	if( !isProcessRightPart ) {
		error( EC_ThereAreNoPartsSeparatorInRules );
	}
	if( HasErrors() ) {
		acc.Empty();
		CVariablesBuilder::Reset();
	} else {
		addRule();
	}
	isProcessRightPart = false;
}

void CFunctionBuilder::AddChar( TChar c )
{
	if( !HasErrors() ) {
		acc.AppendChar( c );
	}
}

void CFunctionBuilder::AddLabel( TLabel label )
{
	if( !HasErrors() ) {
		acc.AppendLabel( label );
	}
}

void CFunctionBuilder::AddNumber( TNumber number )
{
	if( !HasErrors() ) {
		acc.AppendNumber( number );
	}
}

void CFunctionBuilder::AddVariable( TVariableType type, TVariableName name,
	CQualifier* qualifier )
{
	TVariableIndex index = InvalidVariableIndex;
	if( isProcessRightPart ) {
		index = CVariablesBuilder::AddRight( name, type, qualifier );
	} else {
		index = CVariablesBuilder::AddLeft( name, type, qualifier );
	}
	if( !HasErrors() ) {
		if( index == InvalidVariableIndex ) {
			//SetErrors();
		} else {
			acc.AppendVariable( index );
		}
	}
}

void CFunctionBuilder::AddLeftParen()
{
	balanceStack.push( acc.AppendLeftParen() );
}

void CFunctionBuilder::AddRightParen()
{
	CUnitNode* leftParen = 0;
	if( !balanceStack.empty() ) {
		leftParen = balanceStack.top();
	}
	if( leftParen != 0 && leftParen->IsLeftParen() ) {
		leftParen->PairedParen() = acc.AppendRightParen(leftParen);
		balanceStack.pop();
	} else {
		error( EC_RightParenDoesNotMatchLeftParen );
	}
}

void CFunctionBuilder::AddLeftBracket()
{
	if( isProcessRightPart ) {
		balanceStack.push( acc.AppendLeftBracket() );
	} else {
		error( EC_IllegalLeftBracketInLeftPart );
	}
}

void CFunctionBuilder::AddRightBracket()
{	
	if( isProcessRightPart ) {
		CUnitNode* leftBracket = 0;
		if( !balanceStack.empty() ) {
			leftBracket = balanceStack.top();
		}
		if( leftBracket != 0 && leftBracket->IsLeftBracket() ) {
			leftBracket->PairedParen() = acc.AppendRightBracket(leftBracket);
			balanceStack.pop();
		} else {
			error( EC_RightBracketDoesNotMatchLeftBracket );
		}
	} else {
		error( EC_IllegalRightBracketInLeftPart );
	}
}

void CFunctionBuilder::error( TErrorCode errorCode )
{
	switch( errorCode ) {
		case EC_ThereAreNoRulesInFunction:
			CErrorsHelper::Error( "there are no rules in function" );
			break;
		case EC_IllegalLeftBracketInLeftPart:
			CErrorsHelper::Error( "illegal left bracket in left part" );
			break;
		case EC_IllegalRightBracketInLeftPart:
			CErrorsHelper::Error( "illegal right bracket in left part" );
			break;
		case EC_RightParenDoesNotMatchLeftParen:
			CErrorsHelper::Error( "right paren does not match left paren" );
			break;
		case EC_RightBracketDoesNotMatchLeftBracket:
			CErrorsHelper::Error( "right bracket does not match left bracket" );
			break;
		case EC_UnclosedLeftParenInLeftPart:
			CErrorsHelper::Error( "unclosed left paren in left part" );
			break;
		case EC_UnclosedLeftParenInRightPart:
			CErrorsHelper::Error( "unclosed left paren in right part" );
			break;
		case EC_UnclosedLeftBracketInRightPart:
			CErrorsHelper::Error( "unclosed left bracket in right part" );
			break;
		case EC_ThereAreMultiplePartsSeparatorInRules:
			CErrorsHelper::Error( "there are multiple parts separator in rules" );
			break;
		case EC_ThereAreNoPartsSeparatorInRules:
			CErrorsHelper::Error( "there are no parts separator in rules" );
			break;
		default:
			assert( false );
			break;
	}
}

void CFunctionBuilder::emptyStack()
{
	std::stack<CUnitNode*> emptyStack;
	std::swap( balanceStack, emptyStack );
}

void CFunctionBuilder::emptyRules()
{
	if( firstRule != 0 ) {
		while( firstRule != 0 ) {
			CFunctionRule* tmp = firstRule;
			firstRule = firstRule->nextRule;
			delete tmp;
		}
	}
	firstRule = 0;
	lastRule = 0;
}

void CFunctionBuilder::addRule()
{
	CFunctionRule* newRule = new CFunctionRule( isRightDirection );
	leftPart.Move( &(newRule->leftPart) );
	acc.Move( &(newRule->rightPart) );
	CVariablesBuilder::Export( newRule->variables );
	if( firstRule != 0 ) {
		lastRule->nextRule = newRule;
		lastRule = lastRule->nextRule;
	} else {
		firstRule = newRule;
		lastRule = firstRule;
	}
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
