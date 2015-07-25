#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

void PrintRule( const CFunctionRule& rule )
{
	std::cout << "\t";
	if( rule.isRightDirection ) {
		std::cout << "R";
	}
	std::cout << " ";
	PrintUnitList( rule.leftPart, &rule.variables );
	std::cout << "= ";
	PrintUnitList( rule.rightPart );
}

void PrintFunction( const CFunction& function )
{
	assert( function.IsParsed() );
	for( const CFunctionRule* rule = function.firstRule; rule != 0;
		rule = rule->nextRule )
	{
		PrintRule( *rule );
		std::cout << std::endl;
	}
}

//-----------------------------------------------------------------------------

CPreparatoryFunction::CPreparatoryFunction( const CToken& _name ):
	type( PFT_Declared ),
	entry( false ),
	name( _name )
{
}

void CPreparatoryFunction::SetDefined( const CToken& _name )
{
	assert( AreTokenWordsEqual( name, _name ) );
	assert( type == PFT_Declared );
	type = PFT_Defined;
	name = _name;
}

void CPreparatoryFunction::SetOrdinary( CRulePtr& _firstRule )
{
	assert( type == PFT_Defined );
	assert( static_cast<bool>( _firstRule ) );
	type = PFT_Ordinary;
	firstRule.reset( _firstRule.release() );
}

void CPreparatoryFunction::SetEmpty()
{
	assert( type == PFT_Defined );
	type = PFT_Empty;
}

void CPreparatoryFunction::SetEntry( const CToken& _externalName )
{
	assert( type != PFT_External );
	entry = true;
	externalName = _externalName;
}

void CPreparatoryFunction::SetExternal( const CToken& _externalName )
{
	assert( !entry );
	assert( type == PFT_Defined );
	type = PFT_External;
	externalName = _externalName;
}

//-----------------------------------------------------------------------------

CFunctionBuilder::CFunctionBuilder( IErrorHandler* errorHandler ):
	CVariablesBuilder( errorHandler ),
	isProcessRightPart( false ),
	isRightDirection( false ),
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
			//function.SetParsed( &firstRule );
		} else {
			error( EC_ThereAreNoRulesInFunction );
		}
	}
	Reset();
}

void CFunctionBuilder::SetRightDirection()
{
	isRightDirection = true;
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
			acc.Move( leftPart );
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
	firstRule.reset();
	lastRule = 0;
}

void CFunctionBuilder::addRule()
{
	if( static_cast<bool>( firstRule ) ) {
		lastRule->NextRule.reset( new CRule );
		lastRule = lastRule->NextRule.get();
	} else {
		firstRule.reset( new CRule );
		lastRule = firstRule.get();
	}

	lastRule->RightMatching = isRightDirection;
	leftPart.Move( lastRule->Left );
	acc.Move( lastRule->Right );
	CVariablesBuilder::Export( lastRule->Variables );
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
