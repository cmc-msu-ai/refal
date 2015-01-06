#include <bitset>
#include <vector>
#include <Refal2.h>

namespace Refal2 {

void CFunctionCompiler::removeCurrentHole()
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

void CFunctionCompiler::compileRule(CFunctionRule* rule)
{
	firstHole = new CHole;
	rule->leftPart.Move( &(firstHole->hole) );
	rule->variables.Move( &variables );
	
	do {
		// HSCH
		currentHole = firstHole;
		
		// HSCH 1
		do {
			CUnitNode* left = currentHole->hole.GetFirst();
			CUnitNode* right = currentHole->hole.GetLast();
			
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

TVariablesMask CFunctionCompiler::makeVariablesMask(const CHole& hole) const
{
	TVariablesMask variablesMask;
	for( const CUnitNode* i = hole.hole.GetFirst(); i != 0; i = i->Next() ) {
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

void CFunctionCompiler::splitIntoClasses(CHole* const holes)
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

void CFunctionCompiler::matchVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchVE()\n");
	currentHole = firstHole;
	delete detachLeftUnitInCurrentHole();
}

void CFunctionCompiler::matchElement()
{
	//if( currentLeft != currentHole->left || currentRight != currentHole->Right ) {
		/* TODO: SB( currentHole->left, currentHole->Right ) */
	//}
	
	if( currentHole->hole.IsEmpty() ) {
		matchEmptyExpression();
	} else {
		CUnitNode* left = currentHole->hole.GetFirst();
		CUnitNode* right = currentHole->hole.GetLast();
	
		if( left == left && isFreeVE(left) ) {
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

bool CFunctionCompiler::tryMatchLeftVariable(CUnitNode* left)
{
	switch( variables.GetVariable( left->Variable() )->GetType() ) {
		case 's':
			matchLeftS();
			return true;
			break;
		case 'w':
			matchLeftW();
			return true;
			break;
		case 'v':
		case 'e':
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

bool CFunctionCompiler::tryMatchRightVariable(CUnitNode* right)
{
	switch( variables.GetVariable( right->Variable() )->GetType() ) {
		case 's':
			matchRightS();
			return true;
			break;
		case 'w':
			matchRightW();
			return true;
			break;
		case 'v':
		case 'e':
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

void CFunctionCompiler::matchEmptyExpression()
{
	// TODO: action
	printf("CFunctionCompiler::matchEmptyExpression()\n");
	removeCurrentHole();
}

void CFunctionCompiler::matchClosedE()
{
	// TODO: action
	printf("CFunctionCompiler::matchClosedE()\n");
	delete detachLeftUnitInCurrentHole();
	removeCurrentHole();
}

void CFunctionCompiler::matchLeftParens()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftParens()\n");
	
	CUnitNode* begin = currentHole->hole.GetFirst();
	CUnitNode* end = currentHole->hole.GetLast();
	CUnitNode* beginNew = begin->PairedParen()->Next();
	CUnitNode* endNew = 0;
	if( beginNew != 0 ) {
		endNew = end;
		currentHole->hole.Detach( beginNew, endNew );
	}
	
	delete detachLeftUnitInCurrentHole();
	delete detachRightUnitInCurrentHole();
	
	CHole* tmp = new CHole;
	tmp->hole.Assign( beginNew, endNew );
	tmp->nextHole = currentHole->nextHole;
	currentHole->nextHole = tmp;
}

void CFunctionCompiler::matchRightParens()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightParens()\n");
	
	CUnitNode* begin = currentHole->hole.GetFirst();
	CUnitNode* end = currentHole->hole.GetLast();
	CUnitNode* beginNew = 0;
	CUnitNode* endNew = end->PairedParen()->Prev();
	if( endNew != 0 ) {
		beginNew = begin;
		currentHole->hole.Detach( beginNew, endNew );
	}
	
	delete detachLeftUnitInCurrentHole();
	delete detachRightUnitInCurrentHole();
	
	CHole* tmp = new CHole;
	tmp->hole.Assign( beginNew, endNew );
	tmp->nextHole = currentHole->nextHole;
	currentHole->nextHole = tmp;
}

void CFunctionCompiler::matchLeftSymbol()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftSymbol()\n");
	delete detachLeftUnitInCurrentHole();
}

void CFunctionCompiler::matchRightSymbol()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightSymbol()\n");
	delete detachRightUnitInCurrentHole();
}

void CFunctionCompiler::matchLeftS()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftS()\n");
	delete detachLeftUnitInCurrentHole();
}

void CFunctionCompiler::matchRightS()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightS()\n");
	delete detachRightUnitInCurrentHole();
}

void CFunctionCompiler::matchLeftW()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftW()\n");
	delete detachLeftUnitInCurrentHole();
}

void CFunctionCompiler::matchRightW()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightW()\n");
	delete detachRightUnitInCurrentHole();
}

void CFunctionCompiler::matchLeftDuplicateVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchLeftDuplicateVE()\n");
	delete detachLeftUnitInCurrentHole();
}

void CFunctionCompiler::matchRightDuplicateVE()
{
	// TODO: action
	printf("CFunctionCompiler::matchRightDuplicateVE()\n");
	delete detachRightUnitInCurrentHole();
}

} // end of namespace refal2
