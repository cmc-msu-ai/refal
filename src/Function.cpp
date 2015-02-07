#include <stack>
#include <Refal2.h>

namespace Refal2 {

void PrintRule(const CFunctionRule& rule)
{
	printf("\t");
	if( rule.isRightDirection ) {
		printf("R");
	}
	printf(" ");
	PrintUnitList( rule.leftPart, &rule.variables );
	printf("= ");
	PrintUnitList( rule.rightPart );
}

void PrintFunction(const CFunction& function)
{
	for( const CFunctionRule* rule = function.firstRule; rule != 0;
		rule = rule->nextRule )
	{
		PrintRule( *rule );
		printf("\n");
	}
}

CFunctionBuilder::CFunctionBuilder(IFunctionBuilderListener* listener):
	CVariablesBuilder( dynamic_cast<IVariablesBuilderListener*>(listener) ),
	CListener<IFunctionBuilderListener>(listener),
	state(FBS_Direction),
	isRightDirection(false),
	firstRule(0),
	lastRule(0)
{
}

void CFunctionBuilder::Reset()
{
	CVariablesBuilder::Reset();
	state = FBS_Direction;
	acc.Empty();
	emptyRules();
	emptyStack();
}

void CFunctionBuilder::Export(CFunction* function)
{
	assert( state == FBS_Direction );

	if( !HasErrors() ) {
		if( firstRule != 0 ) {
			function->SetParsed( &firstRule );
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

void CFunctionBuilder::AddDirection(bool _isRightDirection)
{
	assert( state == FBS_Direction );
	
	isRightDirection = _isRightDirection;
	state = FBS_Left;
}

void CFunctionBuilder::AddEndOfLeft()
{
	assert( state == FBS_Left );
	
	while( !balanceStack.empty() ) {
		balanceStack.pop();
		error( FBEC_UnclosedLeftParenInLeftPart );
	}
	
	if( HasErrors() ) {
		acc.Empty();
	} else {
		acc.Move( &leftPart );
	}
	
	state = FBS_Right;
}

void CFunctionBuilder::AddEndOfRight()
{
	assert( state == FBS_Right );
	
	while( !balanceStack.empty() ) {
		CUnitNode* unit = balanceStack.top();
		balanceStack.pop();
		error( unit->IsLeftParen() ? FBEC_UnclosedLeftParenInRightPart :
			FBEC_UnclosedLeftBracketInRightPart );
	}
	
	if( HasErrors() ) {
		acc.Empty();
		CVariablesBuilder::Reset();
	} else {
		addRule();
	}
	
	state = FBS_Direction;
}

void CFunctionBuilder::AddLeftVariable(TVariableType type, TVariableName name,
	CQualifier* qualifier)
{
	assert( state == FBS_Left );
	
	TVariableIndex index = CVariablesBuilder::AddLeft( name, type, qualifier );	
	if( !HasErrors() ) {
		if( index == InvalidVariableIndex ) {
			SetErrors();
		} else {
			acc.AppendVariable( index );
		}
	}
}

void CFunctionBuilder::AddRightVariable(TVariableType type, TVariableName name)
{
	assert( state == FBS_Right );
	
	TVariableIndex index = CVariablesBuilder::AddRight( name, type );
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
	assert( state != FBS_Direction );
	
	balanceStack.push( acc.AppendLeftParen() );
}

void CFunctionBuilder::AddRightParen()
{
	assert( state != FBS_Direction );
	
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
	assert( state != FBS_Direction );

	if( state == FBS_Left ) {
		error( FBEC_IllegalLeftBracketInLeftPart );
	} else {
		balanceStack.push( acc.AppendLeftBracket() );
	}
}

void CFunctionBuilder::AddRightBracket()
{
	assert( state != FBS_Direction );
	
	if( state == FBS_Left ) {
		error( FBEC_IllegalRightBracketInLeftPart );
	} else {
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
	}
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
	CVariablesBuilder::Export( &(newRule->variables) );
	
	if( firstRule != 0 ) {
		lastRule->nextRule = newRule;
		lastRule = lastRule->nextRule;
	} else {
		firstRule = newRule;
		lastRule = firstRule;
	}
}

} // end of namespace refal2
