#include <bitset>
#include <vector>
#include <Refal2.h>

namespace Refal2 {

void CLeftPartCompiler::CompileLeftPart(CUnitList* leftPart,
	const bool isRightDirection)
{

}

#if 0
void CLeftPartCompiler::CompileLeftPart(CUnitList* leftPart,
	const bool isRightDirection)
{	
	do {
		// HSCH
		currentHole = firstHole;
		
		// HSCH 1
		do {
			TUnitNode* left = currentHole->hole.GetFirst();
			TUnitNode* right = currentHole->hole.GetLast();
			
			if( isMarkedVariable(left) )
			{
				// HSCH 2
				classes.push_back( CHolesTuple(
					markedStackDepth[left->Variable()], currentHole ) );
				markedVariables.reset( left->Variable() );
				break;
			} else {
				// HSCH 3
				if( left != right && isFreeVE(left) && isFreeVE(right) ) {
					currentHole = currentHole->nextHole;
				} else {
					matchElement();
					currentHole = firstHole;
				}
			}
		} while( currentHole != 0 );
		
		// HSCH 4
		if( firstHole == 0 ) {
			if( !classes.empty() ) {
				// TODO: EOE( stackDepth - classes.back().stackDepth );
				firstHole = classes.back().firstHole;
				classes.pop_back();
			}
		} else {
			matchVE();
		}
	} while( firstHole != 0 );
}

TVariablesMask CLeftPartCompiler::makeVariablesMask(const CHole& hole) const
{
	TVariablesMask variablesMask;
	for( const TUnitNode* i = hole.hole.GetFirst(); i != 0; i = i->Next() ) {
		if( i->IsVariable() && !variables.IsSet( i->Variable() ) ) {
			variablesMask.set( i->Variable() );
		}
	}
	return variablesMask;
}

struct CHoleInfo {
	CHole* hole;
	TVariablesMask variablesMask;
	std::size_t classid;
	
	CHoleInfo(CHole* _hole = 0):
		hole(_hole), classid(0) {}
};

void CLeftPartCompiler::splitIntoClasses(CHole* const holes)
{
	std::vector<CHoleInfo> info;	
	for( CHole* i = holes; i != 0; i = i->nextHole ) {
		info.push_back( CHoleInfo(i) );
		info.back().variablesMask = makeVariablesMask(*i);
	}
	
	std::size_t classid = 0;
	for( std::size_t i = 0; i < info.size() - 1; i++ ) {
		if( info[i].classid == 0 ) {
			info[i].classid = ++classid;
			TVariablesMask variablesMask = info[i].variablesMask;
			
			for( std::size_t j = i + 1; j < info.size(); j++ ) {
				if( info[j].classid == 0 &&
					(info[j].variablesMask & variablesMask).any() )
				{
					info[j].classid = classid;
					variablesMask |= info[j].variablesMask;
					j = i;
				}
			}
		}
	}
	
	if( classid > 1 ) {
		for( std::size_t i = 0; i < classid; i++ ) {
			classes.push_back( CHolesTuple(stackDepth) );
		}
		
		std::size_t offset = classes.size() - classid - 1;
		for( std::size_t i = 0; i < info.size(); i++ ) {
			CHole*& tmp = classes[offset + info[i].classid].firstHole;
			info[i].hole->nextHole = tmp;
			tmp = info[i].hole;
		}
	} else {
		classes.push_back( CHolesTuple(stackDepth, holes) );
	}
}

void CLeftPartCompiler::removeCurrentHole()
{
	if( currentHole == firstHole ) {
		firstHole = currentHole->nextHole;
		delete currentHole;
		currentHole = firstHole;
	} else {
		CHole* i = firstHole; 
		while( i->nextHole != currentHole ) {
			i = i->nextHole;
		}
		
		i->nextHole = currentHole->nextHole;
		delete currentHole;
		currentHole = i;
	}
}

void CLeftPartCompiler::matchVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchVE()\n");
	currentHole = firstHole;
	delete detachLeftUnitInCurrentHole();
}

#endif

void CLeftPartCompiler::matchElement()
{
	if( !hole->CompareLeftAndRight( left, right ) ) {
		// TODO: add SB( hole->GetLeft(), hole->GetRight() );
	}

	if( hole->IsEmpty() ) {
		matchEmptyExpression();
	} else {
		TUnitNode* left = hole->GetFirst();
		TUnitNode* right = hole->GetLast();
	
		if( left == right && isFreeVE(left) ) {
			matchClosedE();
		} else if( left->IsParen() ) {
			matchLeftParens();
		} else if( left->IsSymbol() ) {
			matchLeftSymbol();
		} else if( left->IsVariable() && tryMatchLeftVariable( left ) ) {
		} else if( right->IsParen() ) {
			matchRightParens();
		} else if( right->IsSymbol() ) {
			matchRightSymbol();
		} else if( right->IsVariable() && tryMatchRightVariable( right ) ) {
		} else {
			assert( false );
		}
	}
}

bool CLeftPartCompiler::tryMatchLeftVariable(TUnitNode* left)
{
	switch( variables.GetVariable( left->Variable() )->GetType() ) {
		case VariableTypeS:
			matchLeftS();
			return true;
			break;
		case VariableTypeW:
			matchLeftW();
			return true;
			break;
		case VariableTypeV:
		case VariableTypeE:
			if( variables.IsSet( left->Variable() ) ) {
				matchLeftDuplicateVE();
				return true;
			}
			break;
		default:
			assert( false );
			break;
	}
	return false;
}

bool CLeftPartCompiler::tryMatchRightVariable(TUnitNode* right)
{
	switch( variables.GetVariable( right->Variable() )->GetType() ) {
		case VariableTypeS:
			matchRightS();
			return true;
			break;
		case VariableTypeW:
			matchRightW();
			return true;
			break;
		case VariableTypeV:
		case VariableTypeE:
			if( variables.IsSet( right->Variable() ) ) {
				matchRightDuplicateVE();
				return true;
			}
			break;
		default:
			assert( false );
			break;
	}
	return false;
}

void CLeftPartCompiler::matchEmptyExpression()
{
	// TODO: action
	printf("CFunctionCompiler::matchEmptyExpression()\n");
	//removeHole();
}

void CLeftPartCompiler::matchClosedE()
{
	// TODO: action
	printf("CFunctionCompiler::matchClosedE()\n");
	hole->RemoveFirst();
	//removeHole();
}

void CLeftPartCompiler::matchLeftParens()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftParens()\n");
	
	TUnitNode* begin = hole->GetFirst();
	TUnitNode* end = hole->GetLast();
	TUnitNode* beginNew = begin->PairedParen()->Next();
	TUnitNode* endNew = 0;
	if( beginNew != 0 ) {
		endNew = end;
		hole->Detach( beginNew, endNew );
	}
	
	hole->RemoveFirst();
	hole->RemoveLast();
	
	/*CHole* tmp = new CHole;
	tmp->hole.Assign( beginNew, endNew );
	tmp->nextHole = currentHole->nextHole;
	currentHole->nextHole = tmp;*/
}

void CLeftPartCompiler::matchRightParens()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightParens()\n");
	
	TUnitNode* begin = hole->GetFirst();
	TUnitNode* end = hole->GetLast();
	TUnitNode* beginNew = 0;
	TUnitNode* endNew = end->PairedParen()->Prev();
	if( endNew != 0 ) {
		beginNew = begin;
		hole->Detach( beginNew, endNew );
	}
	
	hole->RemoveFirst();
	hole->RemoveLast();
	
	/*CHole* tmp = new CHole;
	tmp->hole.Assign( beginNew, endNew );
	tmp->nextHole = currentHole->nextHole;
	currentHole->nextHole = tmp;*/
}

void CLeftPartCompiler::matchLeftSymbol()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftSymbol()\n");
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightSymbol()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightSymbol()\n");
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftS()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftS()\n");
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightS()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightS()\n");
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftW()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftW()\n");
	hole->RemoveFirst();

}

void CLeftPartCompiler::matchRightW()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightW()\n");
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftDuplicateVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftDuplicateVE()\n");
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightDuplicateVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightDuplicateVE()\n");
	hole->RemoveLast();
}

void CRightPartCompiler::CompileRightPart(CUnitList* rightPart)
{
}

void CFunctionCompiler::Compile(CFunction* function)
{
	assert( function->IsParsed() );

	for( CFunctionRule* rule = function->firstRule; rule != 0;
		rule = rule->nextRule )
	{
		compileRule( rule );
	}
}

void CFunctionCompiler::compileRule(CFunctionRule* rule)
{
	rule->variables.Move( &variables );
	CompileLeftPart( &rule->leftPart, rule->isRightDirection );
	CompileRightPart( &rule->rightPart );
}

} // end of namespace refal2
