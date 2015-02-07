#pragma once

#include <Refal2.h>

namespace Refal2 {

class COperationsExecuter {
public:
	COperationsExecuter();
	
	void Run( const TLabel entry );

private:
	COperationsExecuter( const COperationsExecuter& );
	COperationsExecuter& operator=( const COperationsExecuter& );
	
	bool doOperation();
	inline void nextOperation();
	
	inline bool checkQualifier( CUnitNode* const node,
		const TQualifierIndex qualifier ) const;
	
	inline void saveToTable( CUnitNode* const node );
	inline void saveToTable( CUnitNode* const nodeA, CUnitNode* const nodeB );
	
	inline bool isEmpty() const;
	
	inline bool shiftLeft();
	inline bool shiftRight();

	inline void saveState( COperationNode* operation );
	void saveState() { saveState( operation ); }
	inline void fail();
	
	inline void matchingComplete();
	inline void doReturn();
	inline void insertJump( const TOperationAddress operationAddress );
	
	inline void setLeftBorder( const TTableIndex tableIndex );
	inline void setRightBorder( const TTableIndex tableIndex );
	
	/* matching empty expression */
	inline void matchEmptyExpression();
	/* matching symbols */
	inline void matchLeftChar( const TChar c );
	inline void matchLeftLabel( const TLabel label );
	inline void matchLeftNumber( const TNumber number );
	inline void matchRightChar( const TChar c );
	inline void matchRightLabel( const TLabel label );
	inline void matchRightNumber( const TNumber number );
	/* matching parens */
	inline void matchLeftParens();
	inline void matchRightParens();
	/* matching S-variables */
	inline void matchLeft_S();
	inline void matchLeftSaveToTable_S();
	inline void matchLeftWithQualifier_S( const TQualifierIndex qualifier );
	inline void matchLeftWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	inline void matchRight_S();
	inline void matchRightSaveToTable_S();
	inline void matchRightWithQualifier_S( const TQualifierIndex qualifier );
	inline void matchRightWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	/* matching duplicate of S-variables */
	inline void matchLeftDuplicate_S( const TTableIndex origin );
	inline void matchLeftDuplicateSaveToTable_S( const TTableIndex origin );
	inline void matchRightDuplicate_S( const TTableIndex origin );
	inline void matchRightDuplicateSaveToTable_S( const TTableIndex origin );
	/* matching W-variables */
	inline void matchLeft_W();
	inline void matchLeftSaveToTable_W();
	inline void matchLeftWithQualifier_W( const TQualifierIndex qualifier );
	inline void matchLeftWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier );
	inline void matchRight_W();
	inline void matchRightSaveToTable_W();
	inline void matchRightWithQualifier_W( const TQualifierIndex qualifier );
	inline void matchRightWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier );
	/* matching duplicate of WV-variables */
	inline void matchLeftDuplicate_WV( const TTableIndex origin );
	inline void matchLeftDuplicateSaveToTable_WV( const TTableIndex origin );
	inline void matchRightDuplicate_WV( const TTableIndex origin );
	inline void matchRightDuplicateSaveToTable_WV( const TTableIndex origin );
	/* matching duplicate of E-variables */
	inline void matchLeftDuplicate_E( const TTableIndex origin );
	inline void matchLeftDuplicateSaveToTable_E( const TTableIndex origin );
	inline void matchRightDuplicate_E( const TTableIndex origin );
	inline void matchRightDuplicateSaveToTable_E( const TTableIndex origin );
	/* matching closed V-variables */
	inline void matchClosed_V();
	inline void matchClosedSaveToTable_V();
	inline void matchClosedWithQualifier_V( const TQualifierIndex qualifier );
	inline void matchClosedWithQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	/* matching closed E-variables */
	inline void matchClosed_E();
	inline void matchClosedSaveToTable_E();
	inline void matchClosedWithQualifier_E( const TQualifierIndex qualifier );
	inline void matchClosedWithQualifierSaveToTable_E(
		const TQualifierIndex qualifier);
	/* matching V-variables by qualifier */
	inline void macthLeftMaxByQualifier_V( const TQualifierIndex qualifier );
	inline void macthLeftMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifier_V( const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	/* matching E-variables by qualifier */
	inline void macthLeftMaxByQualifier_E( const TQualifierIndex qualifier );
	inline void macthLeftMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifier_E( const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	/* match left VE-variable */
	inline void matchLeftBegin_E();
	inline void matchLeftBeginSaveToTable_E();
	inline void matchLeftBegin_V();
	inline void matchLeftBeginSaveToTable_V();
	inline void matchLeftWithQulifierBegin_V( const TQualifierIndex qualifier );
	inline void matchLeftWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	inline void matchLeft_E();
	inline void matchLeftSaveToTable_E();
	inline void matchLeftWithQulifier_E( const TQualifierIndex qualifier );
	inline void matchLeftWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier );
	/* match right VE-variable */
	inline void matchRightBegin_E();
	inline void matchRightBeginSaveToTable_E();
	inline void matchRightBegin_V();
	inline void matchRightBeginSaveToTable_V();
	inline void matchRightWithQulifierBegin_V(
		const TQualifierIndex qualifier );
	inline void matchRightWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	inline void matchRight_E();
	inline void matchRightSaveToTable_E();
	inline void matchRightWithQulifier_E( const TQualifierIndex qualifier );
	inline void matchRightWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier );
	/* making operations */
	inline void insertChar( const TChar c );
	inline void insertLabel( const TLabel label );
	inline void insertNumber( const TNumber number );
	inline void insertLeftParen();
	inline void insertRightParen();
	inline void insertRightBracket();
	inline void move_S( const TTableIndex tableIndex );
	inline void copy_S( const TTableIndex tableIndex );
	inline void move_E( const TTableIndex tableIndex );
	inline void copy_E( const TTableIndex tableIndex );
	inline void move_WV( const TTableIndex tableIndex );
	inline void copy_WV( const TTableIndex tableIndex );
	
	CUnitList fieldOfView;
	CUnitNode* left;
	CUnitNode* right;
	
	CUnitNode* table[4096];
	TTableIndex tableTop;

	COperationNode* operation;

	struct CStackData {
		CUnitNode* left;
		CUnitNode* right;
		TTableIndex tableTop;
		COperationNode* operation;
	};
	CStackData stack[128];
	int stackTop;
	
	CUnitNode* lastAddedLeftParen;
	CUnitNode* lastAddedLeftBracket;
	CUnitNode initialLeftBracket;
};

inline void COperationsExecuter::nextOperation()
{
	operation = operation->Next();
	//if( operation->type == OT_Goto ) {
		// TODO: calc new pointer */
	//}
}

inline bool COperationsExecuter::checkQualifier(CUnitNode* const node,
	const TQualifierIndex qualifier) const
{
	return true;
}

inline void COperationsExecuter::saveToTable(CUnitNode* const node)
{
	table[tableTop] = node;
	tableTop++;
	if( node != 0 ) {
		PrintUnit( *node );
	}
}

inline void COperationsExecuter::saveToTable(CUnitNode* const nodeA,
	CUnitNode* const nodeB)
{
	saveToTable( nodeA );
	saveToTable( nodeB );
	if( nodeA != 0 && nodeB != 0 ) {
		PrintUnitList( nodeA, nodeB, 0 );
	}
}

inline bool COperationsExecuter::isEmpty() const
{
	return ( left->Next() == right );
}

inline bool COperationsExecuter::shiftLeft()
{
	left = left->Next();
	return ( left != right );
}

inline bool COperationsExecuter::shiftRight()
{
	right = right->Prev();
	return ( left != right );
}

inline void COperationsExecuter::saveState( COperationNode* operation )
{
	stack[stackTop].left = left;
	stack[stackTop].right = right;
	stack[stackTop].tableTop = tableTop;
	stack[stackTop].operation = operation;
	stackTop++;
}

inline void COperationsExecuter::fail()
{
	if( stackTop == 0 ) {
		assert( false );
	} else {
		stackTop--;
		left = stack[stackTop].left;
		right = stack[stackTop].right;
		tableTop = stack[stackTop].tableTop;
		operation = stack[stackTop].operation;
	}
}

inline void COperationsExecuter::matchingComplete()
{
	left = table[1];
}

inline void COperationsExecuter::doReturn()
{
	fieldOfView.Remove( table[0]->Prev(), table[1] );
}

inline void COperationsExecuter::insertJump(
	const TOperationAddress operationAddress )
{
	saveState( static_cast<COperationNode*>( operationAddress ) );
}

inline void COperationsExecuter::setLeftBorder(const TTableIndex tableIndex)
{
	left = table[tableIndex];
}

inline void COperationsExecuter::setRightBorder(const TTableIndex tableIndex)
{
	right = table[tableIndex];
}

inline void COperationsExecuter::matchEmptyExpression()
{
	if( !isEmpty() ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftChar(const TChar c)
{
	if( !shiftLeft() || !left->IsChar() || left->Char() != c ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftLabel(const TLabel label)
{
	if( !shiftLeft() || !left->IsLabel() || left->Label() != label ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftNumber(const TNumber number)
{
	if( !shiftLeft() || !left->IsNumber() || left->Number() != number ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightChar(const TChar c)
{
	if( !shiftRight() || !left->IsChar() || left->Char() != c ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightLabel(const TLabel label)
{
	if( !shiftRight() || !left->IsLabel() || left->Label() != label ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightNumber(const TNumber number)
{
	if( !shiftRight() || !left->IsNumber() || left->Number() != number ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftParens()
{
	if( shiftLeft() && left->IsLeftParen() ) {
		right = left->PairedParen();
		saveToTable( left, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightParens()
{
	if( shiftRight() && right->IsRightParen() ) {
		left = right->PairedParen();
		saveToTable( left, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_S()
{
	if( !shiftLeft() || !left->IsSymbol() ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftSaveToTable_S()
{
	if( shiftLeft() && left->IsSymbol() ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQualifier_S(
	const TQualifierIndex qualifier)
{
	if( !shiftLeft() || !left->IsSymbol() || !checkQualifier( left, qualifier ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier)
{
	if( shiftLeft() && left->IsSymbol() && checkQualifier( left, qualifier ) ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRight_S()
{
	if( !shiftRight() || !right->IsSymbol() ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightSaveToTable_S()
{
	if( shiftRight() && right->IsSymbol() ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQualifier_S(
	const TQualifierIndex qualifier)
{
	if( !shiftRight() || !right->IsSymbol() || !checkQualifier( right, qualifier ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier)
{
	if( shiftRight() && right->IsSymbol() && checkQualifier( right, qualifier ) ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftDuplicate_S(const TTableIndex origin)
{
	if( !shiftLeft() || !CompareUnit( *left, *table[origin] ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftDuplicateSaveToTable_S(
	const TTableIndex origin)
{
	if( shiftLeft() && CompareUnit( *left, *table[origin] ) ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightDuplicate_S(const TTableIndex origin)
{
	if( !shiftRight() || !CompareUnit( *right, *table[origin] ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightDuplicateSaveToTable_S(
	const TTableIndex origin)
{
	if( shiftRight() && CompareUnit( *right, *table[origin] ) ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_W()
{
	if( !shiftLeft() ) {
		fail();
		return;
	}
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
}

inline void COperationsExecuter::matchLeftSaveToTable_W()
{
	if( !shiftLeft() ) {
		fail();
		return;
	}
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	saveToTable( left );
}

inline void COperationsExecuter::matchLeftWithQualifier_W(
	const TQualifierIndex qualifier)
{
	if( !shiftLeft() || !checkQualifier( left, qualifier ) ) {
		fail();
		return;
	}
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
}

inline void COperationsExecuter::matchLeftWithQualifierSaveToTable_W(
	const TQualifierIndex qualifier)
{
	if( !shiftLeft() || !checkQualifier( left, qualifier ) ) {
		fail();
		return;
	}
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	saveToTable( left );
}

inline void COperationsExecuter::matchRight_W()
{
	if( !shiftRight() ) {
		fail();
		return;
	}
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
}

inline void COperationsExecuter::matchRightSaveToTable_W()
{
	if( !shiftRight() ) {
		fail();
		return;
	}
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right );
}

inline void COperationsExecuter::matchRightWithQualifier_W(
	const TQualifierIndex qualifier)
{
	if( !shiftRight() || !checkQualifier( right, qualifier ) ) {
		fail();
		return;
	}
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
}

inline void COperationsExecuter::matchRightWithQualifierSaveToTable_W(
	const TQualifierIndex qualifier)
{
	if( !shiftRight() || !checkQualifier( right, qualifier ) ) {
		fail();
		return;
	}
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right );
}

inline void COperationsExecuter::matchLeftDuplicate_WV(
	const TTableIndex origin)
{
	CUnitNode* originLeft = table[origin];
	CUnitNode* const originRight = table[origin + 1];
	for( ; originLeft != originRight->Next();
		originLeft = originLeft->Next() )
	{
		if( !shiftLeft() || !CompareUnit( *left, *originLeft ) ) {
			fail();
			return;
		}
	}
}

inline void COperationsExecuter::matchLeftDuplicateSaveToTable_WV(
	const TTableIndex origin)
{
	CUnitNode* originLeft = table[origin];
	CUnitNode* const originRight = table[origin + 1];
	CUnitNode* valueBegin = left;
	for( ; originLeft != originRight->Next();
		originLeft = originLeft->Next() )
	{
		if( !shiftLeft() || !CompareUnit( *left, *originLeft ) ) {
			fail();
			return;
		}
	}
	saveToTable( valueBegin->Next(), left );
}

inline void COperationsExecuter::matchRightDuplicate_WV(
	const TTableIndex origin)
{
	CUnitNode* const originLeft = table[origin];
	CUnitNode* originRight = table[origin + 1];
	for( ; originRight != originLeft->Prev();
		originRight = originRight->Prev() )
	{
		if( !shiftRight() || !CompareUnit( *right, *originRight ) ) {
			fail();
			return;
		}
	}
}

inline void COperationsExecuter::matchRightDuplicateSaveToTable_WV(
	const TTableIndex origin)
{
	CUnitNode* const originLeft = table[origin];
	CUnitNode* originRight = table[origin + 1];
	CUnitNode* valueEnd = right;
	for( ; originRight != originLeft->Prev();
		originRight = originRight->Prev() )
	{
		if( !shiftRight() || !CompareUnit( *right, *originRight ) ) {
			fail();
			return;
		}
	}
	saveToTable( right, valueEnd->Prev() );
}

inline void COperationsExecuter::matchLeftDuplicate_E(
	const TTableIndex origin)
{
	if( table[origin] != 0 ) {
		matchLeftDuplicate_WV( origin );
	}
}

inline void COperationsExecuter::matchLeftDuplicateSaveToTable_E(
	const TTableIndex origin)
{
	if( table[origin] != 0 ) {
		matchLeftDuplicateSaveToTable_WV( origin );
	}
}

inline void COperationsExecuter::matchRightDuplicate_E(
	const TTableIndex origin)
{
	if( table[origin] != 0 ) {
		matchRightDuplicate_WV( origin );
	}
}

inline void COperationsExecuter::matchRightDuplicateSaveToTable_E(
	const TTableIndex origin)
{
	if( table[origin] != 0 ) {
		matchRightDuplicateSaveToTable_WV( origin );
	}
}

inline void COperationsExecuter::matchClosed_V()
{
	if( isEmpty() ) {
		fail();
	} else {
		matchClosed_E();
	}
}

inline void COperationsExecuter::matchClosedSaveToTable_V()
{
	saveToTable( left->Next(), right->Prev() );
	matchClosed_V();
}

inline void COperationsExecuter::matchClosedWithQualifier_V(
	const TQualifierIndex qualifier)
{
	if( isEmpty() ) {
		fail();
	} else {
		do {
			left = left->Next();
			if( !checkQualifier( left, qualifier ) ) {
				fail();
				return;
			}
		} while( left != right->Prev() );
	}
}

inline void COperationsExecuter::matchClosedWithQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
	saveToTable( left->Next(), right->Prev() );
	matchClosedWithQualifier_V( qualifier );
}

inline void COperationsExecuter::matchClosed_E()
{
	while( !isEmpty() ) {
		left = left->Next();
	}
}

inline void COperationsExecuter::matchClosedSaveToTable_E()
{
	if( isEmpty() ) {
		saveToTable( 0, 0 );
	} else {
		saveToTable( left->Next(), right->Prev() );
		matchClosed_E();
	}
}

inline void COperationsExecuter::matchClosedWithQualifier_E(
	const TQualifierIndex qualifier)
{
	if( !isEmpty() ) {
		matchClosedWithQualifier_V( qualifier );
	}
}

inline void COperationsExecuter::matchClosedWithQualifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	if( isEmpty() ) {
		saveToTable( 0, 0 );
	} else {
		matchClosedWithQualifierSaveToTable_V( qualifier );
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifier_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
	if( left == first ) {
		fail();
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
	if( left == first ) {
		fail();
	} else {
		saveToTable( first->Next(), left );
	}
}

inline void COperationsExecuter::macthRightMaxByQualifier_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
	if( right == last ) {
		fail();
	}
}

inline void COperationsExecuter::macthRightMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
	if( right == last ) {
		fail();
	} else {
		saveToTable( right, last->Prev() );
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifier_E(
	const TQualifierIndex qualifier)
{
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
	if( left == first ) {
		saveToTable( 0, 0 );
	} else {
		saveToTable( first->Next(), left );
	}
}

inline void COperationsExecuter::macthRightMaxByQualifier_E(
	const TQualifierIndex qualifier)
{
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
}

inline void COperationsExecuter::macthRightMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
	if( right == last ) {
		saveToTable( 0, 0 );
	} else {
		saveToTable( right, last->Prev() );
	}
}

inline void COperationsExecuter::matchLeftBegin_E()
{
	nextOperation();
	saveState();
	nextOperation();
}

inline void COperationsExecuter::matchLeftBeginSaveToTable_E()
{
	nextOperation();
	saveState();
	saveToTable( 0, 0 );
	nextOperation();
}

inline void COperationsExecuter::matchLeftBegin_V()
{
	if( shiftLeft() ) {
		nextOperation();
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftBeginSaveToTable_V()
{
	if( shiftLeft() ) {
		nextOperation();
		saveState();
		saveToTable( left, left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifierBegin_V(
	const TQualifierIndex qualifier)
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		nextOperation();
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier)
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		nextOperation();
		saveState();
		saveToTable( left, left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_E()
{
	if( shiftLeft() ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftSaveToTable_E()
{
	if( shiftLeft() ) {
		saveState();
		saveToTable( left );
		tableTop++;
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifier_E(
	const TQualifierIndex qualifier)
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		saveState();
		saveToTable( left );
		tableTop++;
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightBegin_E()
{
	nextOperation();
	saveState();
	nextOperation();
}

inline void COperationsExecuter::matchRightBeginSaveToTable_E()
{
	nextOperation();
	saveState();
	saveToTable( 0, 0 );
	nextOperation();
}

inline void COperationsExecuter::matchRightBegin_V()
{
	if( shiftRight() ) {
		nextOperation();
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightBeginSaveToTable_V()
{
	nextOperation();
	shiftRight();
	saveState();
	saveToTable( right, right );
}

inline void COperationsExecuter::matchRightWithQulifierBegin_V(
	const TQualifierIndex qualifier)
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		nextOperation();
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier)
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		nextOperation();
		saveState();
		saveToTable( right, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRight_E()
{
	if( shiftRight() ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightSaveToTable_E()
{
	if( shiftRight() ) {
		saveState();
		tableTop++;
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQulifier_E(
	const TQualifierIndex qualifier)
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		saveState();
		tableTop++;
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::insertChar( const TChar c )
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_Char ) );
	left->Char() = c;
}

inline void COperationsExecuter::insertLabel( const TLabel label )
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_Label ) );
	left->Label() = label;
}

inline void COperationsExecuter::insertNumber( const TNumber number )
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_Number ) );
	left->Number() = number;
}

inline void COperationsExecuter::insertLeftParen()
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_LeftParen ) );
	left->PairedParen() = lastAddedLeftParen;
	lastAddedLeftParen = left;
}

inline void COperationsExecuter::insertRightParen()
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_RightParen ) );
	CUnitNode* leftParen = lastAddedLeftParen;
	lastAddedLeftParen = lastAddedLeftParen->PairedParen();
	left->PairedParen() = leftParen;
	leftParen->PairedParen() = left;
}

inline void COperationsExecuter::insertRightBracket()
{
	left = fieldOfView.InsertAfter( left, CUnit( UT_RightBracket ) );
	CUnitNode* leftBracket = lastAddedLeftParen;
	lastAddedLeftParen = lastAddedLeftParen->PairedParen();
	left->PairedParen() = leftBracket;
	lastAddedLeftBracket->PairedParen() = left;
	lastAddedLeftBracket = leftBracket;
}

inline void COperationsExecuter::move_S( const TTableIndex tableIndex )
{
	left = fieldOfView.Move( left, table[tableIndex] );
}

inline void COperationsExecuter::copy_S( const TTableIndex tableIndex )
{
	left = fieldOfView.Copy( left, table[tableIndex] );
}

inline void COperationsExecuter::move_E( const TTableIndex tableIndex )
{
	if( table[tableIndex] != 0 ) {
		left = fieldOfView.Move( left, table[tableIndex],
			table[tableIndex + 1] );
	}
}

inline void COperationsExecuter::copy_E( const TTableIndex tableIndex )
{
	if( table[tableIndex] != 0 ) {
		left = fieldOfView.Move( left, table[tableIndex],
			table[tableIndex + 1] );
	}
}

inline void COperationsExecuter::move_WV( const TTableIndex tableIndex )
{
	left = fieldOfView.Move( left, table[tableIndex], table[tableIndex + 1] );
}

inline void COperationsExecuter::copy_WV( const TTableIndex tableIndex )
{
	left = fieldOfView.Move( left, table[tableIndex], table[tableIndex + 1] );
}

} // end of namespace refal2
