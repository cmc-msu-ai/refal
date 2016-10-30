#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CHole

CHole::CHole( CUnitList& hole, TTableIndex _left, TTableIndex _right ) :
	left( _left ),
	right( _right )
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

//-----------------------------------------------------------------------------
// CLeftPartCompiler

CLeftPartCompiler::CLeftPartCompiler() :
	top( 0 ),
	maxTableSize( 0 ),
	hole( 0 )
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
	bool isRightDirection )
{
	top = 2;
	holes.Append( CHole( leftPart, 0 /* left */, 1 /* right */ ) );
	lastUsedHole = holes.GetFirst();

	while( true ) {
		hole = holes.GetFirst();

		while( hole != 0 ) {
			if( hole->GetFirst() != hole->GetLast()
				&& isFreeVE( hole->GetFirst() ) && isFreeVE( hole->GetLast() ) )
			{
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

	if( top > maxTableSize ) {
		maxTableSize = top;
	}
}

void CLeftPartCompiler::removeHole()
{
	if( lastUsedHole == hole ) {
		lastUsedHole = 0;
	}

	CHoleNode* nextHole = hole->Next();
	holes.Remove( hole );
	hole = nextHole;
}

bool CLeftPartCompiler::setVariable(const TVariableIndex variableIndex)
{
	if( !variables.IsFull( variableIndex ) ) {
		variables.Set( variableIndex, top );
		top++;
		if( !variables.GetVariable( variableIndex ).TypeIs( VT_S ) ) {
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
	assert( hole != 0 );
	if( hole != lastUsedHole ) {
		if( lastUsedHole != 0 ) {
			COperationsBuilder::AddSaveLeftRight();
			lastUsedHole->SetLeft( top );
			top++;
			lastUsedHole->SetRight( top );
			top++;
		}
		COperationsBuilder::AddSetLeftBorder( hole->GetLeft() );
		COperationsBuilder::AddSetRightBorder( hole->GetRight() );
	}
	lastUsedHole = hole;
}

void CLeftPartCompiler::matchVE(const bool isRightDirection)
{
	checkBorders();
	if( isRightDirection ) {
		TVariableIndex variableIndex = hole->GetLast()->Variable();

		if( variables.GetVariable( variableIndex ).TypeIs( VT_E ) ) {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchRight_E );
		} else {
			matchVariable( variableIndex,
				&COperationsBuilder::AddMatchRight_V );
		}

		hole->RemoveLast();
	} else {
		TVariableIndex variableIndex = hole->GetFirst()->Variable();

		if( variables.GetVariable( variableIndex ).TypeIs( VT_E ) ) {
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
		
		if( left == right && isFreeVE( left ) ) {
			if( variables.GetVariable( left->Variable() ).TypeIs( VT_V ) ) {
				matchClosedV();
			} else {
				matchClosedE();
			}
		} else if( left->IsParen() ) {
			matchLeftParens();
		} else if( left->IsSymbol() ) {
			matchLeftSymbol();
		} else if( left->IsVariable() && tryMatchLeftVariable() ) {
		} else if( right->IsParen() ) {
			matchRightParens();
		} else if( right->IsSymbol() ) {
			matchRightSymbol();
		} else if( right->IsVariable() && tryMatchRightVariable() ) {
		} else {
			assert( false );
		}
	}
}

bool CLeftPartCompiler::tryMatchLeftVariable()
{
	assert( hole->GetFirst()->IsVariable() );
	const TVariableIndex variable = hole->GetFirst()->Variable();
	if( variables.IsSet( variable ) ) {
		// match duplicate
		switch( variables.GetVariable( variable ).GetType() ) {
			case VT_S:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchLeftDuplicate_S );
				break;
			case VT_W:
			case VT_V:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchLeftDuplicate_WV );
				break;
			case VT_E:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchLeftDuplicate_E );
				break;
			case VT_None:
			default:
				assert( false );
				break;
		}
	} else {
		switch( variables.GetVariable( variable ).GetType() ) {
			case VT_S:
				matchVariable( variable,
					&COperationsBuilder::AddMatchLeft_S );
				break;
			case VT_W:
				matchVariable( variable,
					&COperationsBuilder::AddMatchLeft_W );
				break;
			case VT_V:
			case VT_E:
				return false;
			case VT_None:
			default:
				assert( false );
				break;
		}
	}
	hole->RemoveFirst();
	return true;
}

bool CLeftPartCompiler::tryMatchRightVariable()
{
	assert( hole->GetLast()->IsVariable() );
	const TVariableIndex variable = hole->GetLast()->Variable();
	if( variables.IsSet( variable ) ) {
		// match duplicate
		switch( variables.GetVariable( variable ).GetType() ) {
			case VT_S:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchRightDuplicate_S );
				break;
			case VT_W:
			case VT_V:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchRightDuplicate_WV );
				break;
			case VT_E:
				matchDuplicateVariable( variable,
					&COperationsBuilder::AddMatchRightDuplicate_E );
				break;
			case VT_None:
			default:
				assert( false );
				break;
		}
	} else {
		switch( variables.GetVariable( variable ).GetType() ) {
			case VT_S:
				matchVariable( variable,
					&COperationsBuilder::AddMatchRight_S );
				break;
			case VT_W:
				matchVariable( variable,
					&COperationsBuilder::AddMatchRight_W );
				break;
			case VT_V:
			case VT_E:
				return false;
			case VT_None:
			default:
				assert( false );
				break;
		}
	}
	hole->RemoveLast();
	return true;
}

void CLeftPartCompiler::matchEmptyExpression()
{
	COperationsBuilder::AddMatchEmptyExpression();
	removeHole();
}

void CLeftPartCompiler::matchClosedV()
{
	matchVariable( hole->GetFirst()->Variable(),
		&COperationsBuilder::AddMatchClosed_V );
	hole->RemoveFirst();
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

	holes.InsertAfter( hole, CHole( newHole, top, top + 1 ) );
	top += 2;
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

	holes.InsertBefore( hole, CHole( newHole, top, top + 1 ) );
	top += 2;
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

void CLeftPartCompiler::matchLeftDuplicateVE()
{
	TVariableIndex variableIndex = hole->GetFirst()->Variable();
	if( variables.GetVariable( variableIndex ).TypeIs( VT_V ) ) {
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
	if( variables.GetVariable( variableIndex ).TypeIs( VT_V ) ) {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchRightDuplicate_WV );
	} else {
		matchDuplicateVariable( variableIndex,
			&COperationsBuilder::AddMatchRightDuplicate_E );
	}
	hole->RemoveLast();
}

//-----------------------------------------------------------------------------
// CRightPartCompiler

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
					if( variable.TypeIs( VT_S ) ) {
						COperationsBuilder::AddMove_S( valueIndex );
					} else if( variable.TypeIs( VT_E ) ) {
						COperationsBuilder::AddMove_E( valueIndex );
					} else { // type WV
						COperationsBuilder::AddMove_WV( valueIndex );
					}
				} else {
					if( variable.TypeIs( VT_S ) ) {
						COperationsBuilder::AddCopy_S( valueIndex );
					} else if( variable.TypeIs( VT_E ) ) {
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
// CCompilationContext

void CCompilationContext::Reset()
{
	maxStackDepth = 0;
	maxTableSize = 0;
	operationsHolder.Empty();
}

void CCompilationContext::SaveOperations( COperationList& operations )
{
	assert( !operations.IsEmpty() );
	operationsHolder.Append( operations );
}

void CCompilationContext::SetStackDepth( TStackIndex stackDepth )
{
	if( stackDepth > maxStackDepth ) {
		maxStackDepth = stackDepth;
	}
}

void CCompilationContext::SetTableSize( TTableIndex tableSize )
{
	if( tableSize > maxTableSize ) {
		maxTableSize = tableSize;
	}
}

//-----------------------------------------------------------------------------
// CFunctionCompiler

CFunctionCompiler::CFunctionCompiler(
		CCompilationContext& _compilationContext ) :
	compilationContext( _compilationContext ),
	wasFinalized( false )
{
}

void CFunctionCompiler::CompileRule( CRule& rule )
{
	assert( !wasFinalized );
	rule.Variables.Move( variables );
	CompileLeftPart( rule.Left, rule.RightMatching );
	COperationsBuilder::AddMatchingComplete();
	CompileRightPart( rule.Right );
	COperationsBuilder::AddReturn();
}

TOperationAddress CFunctionCompiler::FinalizeCompilation()
{
	assert( !wasFinalized );
	wasFinalized = true;
	compilationContext.SetStackDepth( COperationsBuilder::MaxStackDepth() );
	compilationContext.SetTableSize( CLeftPartCompiler::MaxTableSize() );
	COperationList functionOperations;
	COperationsBuilder::Export( functionOperations );
	const TOperationAddress resultOperation = functionOperations.GetFirst();
	assert( resultOperation != nullptr );
	compilationContext.SaveOperations( functionOperations );
	return resultOperation;
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
