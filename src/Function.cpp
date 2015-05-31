#include <stack>
#include <Refal2.h>

namespace Refal2 {

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
	for( const CFunctionRule* rule = function.firstRule; rule != 0;
		rule = rule->nextRule )
	{
		PrintRule( *rule );
		printf( "\n" );
	}
}

CFunctionBuilder::CFunctionBuilder(IFunctionBuilderListener* listener):
	CVariablesBuilder( dynamic_cast<IVariablesBuilderListener*>( listener ) ),
	CListener<IFunctionBuilderListener>( listener ),
	isProcessRightPart( false ),
	isRightDirection( false ),
	firstRule( 0 ),
	lastRule( 0 )
{
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

void CFunctionBuilder::Export( CFunction& function )
{
	// todo: check rule
	if( !HasErrors() ) {
		if( firstRule != 0 ) {
			function.SetParsed( &firstRule );
		} else {
			error( FBEC_ThereAreNoRulesInFunction );
		}
	}
	Reset();
}

void CFunctionBuilder::OnErrors()
{
	emptyRules();
	CVariablesBuilder::OnErrors();
}

void CFunctionBuilder::AddEndOfLeft()
{
	if( isProcessRightPart ) {
		error( FBEC_ThereAreMultiplePartsSeparatorInRules );
	} else {
		while( !balanceStack.empty() ) {
			balanceStack.pop();
			error( FBEC_UnclosedLeftParenInLeftPart );
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
		error( unit->IsLeftParen() ? FBEC_UnclosedLeftParenInRightPart :
			FBEC_UnclosedLeftBracketInRightPart );
	}
	if( !isProcessRightPart ) {
		error( FBEC_ThereAreNoPartsSeparatorInRules );
	}
	if( HasErrors() ) {
		acc.Empty();
		CVariablesBuilder::Reset();
	} else {
		addRule();
	}
	isProcessRightPart = false;
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
			SetErrors();
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
		error( FBEC_RightParenDoesNotMatchLeftParen );
	}
}

void CFunctionBuilder::AddLeftBracket()
{
	if( isProcessRightPart ) {
		balanceStack.push( acc.AppendLeftBracket() );
	} else {
		error( FBEC_IllegalLeftBracketInLeftPart );
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
			error( FBEC_RightBracketDoesNotMatchLeftBracket );
		}
	} else {
		error( FBEC_IllegalRightBracketInLeftPart );
	}
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

} // end of namespace refal2
