#include <Refal2.h>

namespace Refal2 {

CHole::CHole( CUnitList& hole, const TTableIndex _left,
		const TTableIndex _right ):
	left( _left ), right( _right )
{
	hole.Move( *this );
}

CHole::CHole(CUnitNode* const first, CUnitNode* const last,
		const TTableIndex _left, const TTableIndex _right):
	CUnitList( first, last ), left( _left ), right( _right )
{
}

CHole::CHole( const CHole& hole )
	: left( hole.left ), right( hole.right )
{
	const_cast<CHole&>( hole ).Move( *this );
}

CHole& CHole::operator=( const CHole& hole )
{
	left = hole.left;
	right = hole.right;
	const_cast<CHole&>( hole ).Move( *this );
	return *this;
}

CLeftPartCompiler::CLeftPartCompiler():
	top(0), left(0), right(0), hole(0)
{
}

#if 0
CVariablesMask CLeftPartCompiler::makeVariablesMask(const CHole& hole) const
{
	CVariablesMask variablesMask;
	for( const CUnitNode* i = hole.hole.GetFirst(); i != 0; i = i->Next() ) {
		if( i->IsVariable() && !variables.IsSet( i->Variable() ) ) {
			variablesMask.set( i->Variable() );
		}
	}
	return variablesMask;
}

struct CHoleInfo {
	CHole* hole;
	CVariablesMask variablesMask;
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
			CVariablesMask variablesMask = info[i].variablesMask;
			
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
#endif

void CLeftPartCompiler::CompileLeftPart( CUnitList& leftPart,
	const bool isRightDirection )
{
	left = 0;
	right = 1;
	top = 2;
	holes.Append( CHole( leftPart, left, right ) );

	while( true ) {
		hole = holes.GetFirst();

		while( hole != 0 ) {
			CUnitNode* left = hole->GetFirst();
			CUnitNode* right = hole->GetLast();

			if( left != right && isFreeVE( left ) && isFreeVE( right ) ) {
				hole = hole->Next();
			} else {
				matchElement();
				hole = holes.GetFirst();
			}
		}

		if( holes.IsEmpty() ) {
			break;
		} else {
			hole = holes.GetFirst();
			matchVE( isRightDirection );
		}
	}
}

void CLeftPartCompiler::removeHole()
{
	CHoleNode* nextHole = hole->Next();
	holes.Remove( hole );
	hole = nextHole;
}

bool CLeftPartCompiler::setVariable(const TVariableIndex variableIndex)
{
	if( !variables.IsFull( variableIndex ) ) {
		variables.Set( variableIndex, top );
		top++;
		if( !variables.GetVariable( variableIndex ).TypeIs( VariableTypeS ) ) {
			top++;
		}
		return true;
	}
	return false;
}

void CLeftPartCompiler::matchVariable(const TVariableIndex variableIndex,
	const TMatchFunction function)
{
	(this->*function)( variables.GetVariable( variableIndex ).GetQualifier(),
		setVariable( variableIndex ) );
}

void CLeftPartCompiler::matchDuplicateVariable(
	const TVariableIndex variableIndex, const TMatchDuplicateFunction function)
{
	(this->*function)( variables.GetMainValue( variableIndex ),
		setVariable( variableIndex ) );
}

void CLeftPartCompiler::checkBorders()
{
	if( left != hole->GetLeft() ) {
		left = hole->GetLeft();
		COperationsBuilder::AddSetLeftBorder( left );
	}
	if( right != hole->GetRight() ) {
		right = hole->GetRight();
		COperationsBuilder::AddSetRightBorder( right );
	}
}

void CLeftPartCompiler::matchVE(const bool isRightDirection)
{
	checkBorders();
	if( isRightDirection ) {
		TVariableIndex variableIndex = hole->GetLast()->Variable();

		if( variables.GetVariable( variableIndex ).TypeIs( VariableTypeE ) ) {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchRight_E );
		} else {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchRight_V );
		}

		hole->RemoveLast();
	} else {
		TVariableIndex variableIndex = hole->GetFirst()->Variable();

		if( variables.GetVariable( variableIndex ).TypeIs( VariableTypeE ) ) {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchLeft_E );
		} else {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchLeft_V );
		}

		hole->RemoveFirst();
	}
}

void CLeftPartCompiler::matchElement()
{
	checkBorders();
	if( hole->IsEmpty() ) {
		matchEmptyExpression();
	} else {
		CUnitNode* left = hole->GetFirst();
		CUnitNode* right = hole->GetLast();
		
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

bool CLeftPartCompiler::tryMatchLeftVariable(CUnitNode* left)
{
	switch( variables.GetVariable( left->Variable() ).GetType() ) {
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

bool CLeftPartCompiler::tryMatchRightVariable(CUnitNode* right)
{
	switch( variables.GetVariable( right->Variable() ).GetType() ) {
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
	COperationsBuilder::AddMatchEmptyExpression();
	removeHole();
}

void CLeftPartCompiler::matchClosedE()
{
	matchVariable( hole->GetFirst()->Variable(),
		&COperationsBuilder::AddMatchClosed_E );
	hole->RemoveFirst();
	removeHole();
}

void CLeftPartCompiler::matchLeftParens()
{
	COperationsBuilder::AddMatchLeftParens();
	
	CUnitNode* begin = hole->GetFirst();
	CUnitNode* end = hole->GetLast();
	CUnitNode* beginNew = begin->PairedParen()->Next();
	CUnitNode* endNew = 0;

	CUnitList newHole;
	if( beginNew != 0 ) {
		endNew = end;
		hole->Detach( beginNew, endNew );
		newHole.Assign( beginNew, endNew );
	}
	
	hole->RemoveFirst();
	hole->RemoveLast();
	
	TTableIndex oldRight = right;
	left = top;
	top++;
	right = top;
	top++;
	hole->SetLeft( left );
	hole->SetRight( right );
	
	holes.InsertAfter( hole, CHole( newHole, right, oldRight ) );
}

void CLeftPartCompiler::matchRightParens()
{
	COperationsBuilder::AddMatchRightParens();
	
	CUnitNode* begin = hole->GetFirst();
	CUnitNode* end = hole->GetLast();
	CUnitNode* beginNew = 0;
	CUnitNode* endNew = end->PairedParen()->Prev();
	
	CUnitList newHole;
	if( endNew != 0 ) {
		beginNew = begin;
		hole->Detach( beginNew, endNew );
		newHole.Assign( beginNew, endNew );
	}
	
	hole->RemoveFirst();
	hole->RemoveLast();
	
	TTableIndex oldLeft = left;
	left = top;
	top++;
	right = top;
	top++;
	hole->SetLeft( left );
	hole->SetRight( right );
	
	holes.InsertBefore( hole, CHole( newHole, oldLeft, left ) );
}

void CLeftPartCompiler::matchLeftSymbol()
{
	CUnitNode unit = *hole->GetFirst();
	switch( unit.GetType() ) {
		case UT_Char:
			COperationsBuilder::AddMatchLeftChar( unit.Char() );
			break;
		case UT_Label:
			COperationsBuilder::AddMatchLeftLabel( unit.Label() );
			break;
		case UT_Number:
			COperationsBuilder::AddMatchLeftNumber( unit.Number() );
			break;
		default:
			assert( false );
			break;
	}
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightSymbol()
{
	CUnitNode unit = *hole->GetLast();
	switch( unit.GetType() ) {
		case UT_Char:
			COperationsBuilder::AddMatchRightChar( unit.Char() );
			break;
		case UT_Label:
			COperationsBuilder::AddMatchRightLabel( unit.Label() );
			break;
		case UT_Number:
			COperationsBuilder::AddMatchRightNumber( unit.Number() );
			break;
		default:
			assert( false );
			break;
	}
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftS()
{
	matchVariable( hole->GetFirst()->Variable(),
		&COperationsBuilder::AddMatchLeft_S );
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightS()
{
	matchVariable( hole->GetLast()->Variable(),
		&COperationsBuilder::AddMatchRight_S );
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftW()
{
	matchVariable( hole->GetFirst()->Variable(),
		&COperationsBuilder::AddMatchLeft_W );
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightW()
{
	matchVariable( hole->GetLast()->Variable(),
		&COperationsBuilder::AddMatchRight_W );
	hole->RemoveLast();
}

void CLeftPartCompiler::matchLeftDuplicateVE()
{
	TVariableIndex variableIndex = hole->GetFirst()->Variable();
	if( variables.GetVariable( variableIndex ).TypeIs( VariableTypeV ) ) {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchLeftDuplicate_WV );
	} else {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchLeftDuplicate_E );
	}
	hole->RemoveFirst();
}

void CLeftPartCompiler::matchRightDuplicateVE()
{
	TVariableIndex variableIndex = hole->GetLast()->Variable();
	if( variables.GetVariable( variableIndex ).TypeIs( VariableTypeV ) ) {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchRightDuplicate_WV );
	} else {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchRightDuplicate_E );
	}
	hole->RemoveLast();
}

void CRightPartCompiler::CompileRightPart( CUnitList& rightPart )
{
	CUnitList hole;
	rightPart.Move( hole );
	
	while( !hole.IsEmpty() ) {
		CUnitNode unit = *hole.GetFirst();
		hole.RemoveFirst();
		
		switch( unit.GetType() ) {
			case UT_Char:
				COperationsBuilder::AddInsertChar( unit.Char() );
				break;
			case UT_Label:
				COperationsBuilder::AddInsertLabel( unit.Label() );
				break;
			case UT_Number:
				COperationsBuilder::AddInsertNumber( unit.Number() );
				break;
			case UT_Variable:
			{
				TVariableIndex variableIndex = unit.Variable();
				const CVariable& variable =
					variables.GetVariable( variableIndex );
				TTableIndex valueIndex;
				
				if( variables.Get( variableIndex, valueIndex ) ) {
					if( variable.TypeIs( VariableTypeS ) ) {
						COperationsBuilder::AddMove_S( valueIndex );
					} else if( variable.TypeIs( VariableTypeE ) ) {
						COperationsBuilder::AddMove_E( valueIndex );
					} else { // type WV
						COperationsBuilder::AddMove_WV( valueIndex );
					}
				} else {
					if( variable.TypeIs( VariableTypeS ) ) {
						COperationsBuilder::AddCopy_S( valueIndex );
					} else if( variable.TypeIs( VariableTypeE ) ) {
						COperationsBuilder::AddCopy_E( valueIndex );
					} else { // type WV
						COperationsBuilder::AddCopy_WV( valueIndex );
					}
				}
				break;
			}
			case UT_LeftParen:
			case UT_LeftBracket:
				COperationsBuilder::AddInsertLeftParen();
				break;
			case UT_RightParen:
				COperationsBuilder::AddInsertRightParen();
				break;
			case UT_RightBracket:
				COperationsBuilder::AddInsertRightBracket();
				break;
			default:
				assert( false );
		}
	}
}

//-----------------------------------------------------------------------------

CFunctionCompiler::CFunctionCompiler( COperationList& _operationsHolder ) :
	operationsHolder( _operationsHolder )
{
}

void CFunctionCompiler::CompileRule( CRule& rule )
{
	rule.Variables.Move( variables );
	CompileLeftPart( rule.Left, rule.RightMatching );
	COperationsBuilder::AddMatchingComplete();
	CompileRightPart( rule.Right );
	COperationsBuilder::AddReturn();
}

TOperationAddress CFunctionCompiler::GetFirstOperation()
{
	COperationNode* lastOperationNode = operationsHolder.GetLast();
	COperationsBuilder::Export( operationsHolder );
	TOperationAddress resultOperation = nullptr;
	if( lastOperationNode == nullptr ) {
		resultOperation = operationsHolder.GetFirst();
	} else {
		resultOperation = lastOperationNode->Next();
	}
	assert( resultOperation != nullptr );
	return resultOperation;
}

} // end of namespace refal2
