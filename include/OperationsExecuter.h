#pragma once

#include <Refal2.h>

namespace Refal2 {

class CContext {

};

class COrdinaryExecuter {
public:
	static bool Execute( CUnitList& argument, CUnitList& result );
};

enum TExecutionResult {
	ER_OK = 0,
	ER_RecognitionImpossible,
	ER_CallEmptyFunction,
	ER_LostFunctionLabel,
	ER_WrongArgumentOfExternalFunction
};

class COperationsExecuter {
public:
	static TExecutionResult Run( const TLabel entry, CUnitList& fieldOfView,
		CUnitNode*& errorCall );
	
private:
	explicit COperationsExecuter( const TLabel entry );
	COperationsExecuter( const COperationsExecuter& );
	COperationsExecuter& operator=( const COperationsExecuter& );
	
	void restoreLeftBrackets();
	void doFunction();
	void doFunctionBody();
	inline void nextOperation();
	
	inline bool checkQualifier( CUnitNode* const node,
		const TQualifierIndex qualifier ) const;
	
	inline void saveToTable( CUnitNode* const node );
	inline void saveToTable( CUnitNode* const nodeA, CUnitNode* const nodeB );
	
	inline bool isEmpty() const;
	
	inline bool shiftLeft();
	inline bool shiftRight();
	
	inline void saveState( COperationNode* operationForSaving );
	void saveState() { saveState( operation ); }
	inline bool fail();
	
	inline void matchingComplete();
	inline void doReturn();
	inline void insertJump( const TOperationAddress operationAddress );
	inline void decrementStackDepth( const TUint32 count );
	inline void setLeftBorder( const TTableIndex tableIndex );
	inline void setRightBorder( const TTableIndex tableIndex );
	// matching empty expression
	inline bool matchEmptyExpression();
	// matching symbols
	inline bool matchLeftChar( const TChar c );
	inline bool matchLeftLabel( const TLabel label );
	inline bool matchLeftNumber( const TNumber number );
	inline bool matchRightChar( const TChar c );
	inline bool matchRightLabel( const TLabel label );
	inline bool matchRightNumber( const TNumber number );
	// matching parens
	inline bool matchLeftParens();
	inline bool matchRightParens();
	// matching S-variables
	inline bool matchLeft_S();
	inline bool matchLeftSaveToTable_S();
	inline bool matchLeftWithQualifier_S( const TQualifierIndex qualifier );
	inline bool matchLeftWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	inline bool matchRight_S();
	inline bool matchRightSaveToTable_S();
	inline bool matchRightWithQualifier_S( const TQualifierIndex qualifier );
	inline bool matchRightWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	// matching duplicate of S-variables
	inline bool matchLeftDuplicate_S( const TTableIndex origin );
	inline bool matchLeftDuplicateSaveToTable_S( const TTableIndex origin );
	inline bool matchRightDuplicate_S( const TTableIndex origin );
	inline bool matchRightDuplicateSaveToTable_S( const TTableIndex origin );
	// matching W-variables
	inline bool matchLeft_W();
	inline bool matchLeftSaveToTable_W();
	inline bool matchLeftWithQualifier_W( const TQualifierIndex qualifier );
	inline bool matchLeftWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier );
	inline bool matchRight_W();
	inline bool matchRightSaveToTable_W();
	inline bool matchRightWithQualifier_W( const TQualifierIndex qualifier );
	inline bool matchRightWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier );
	// matching duplicate of WV-variables
	inline bool matchLeftDuplicate_WV( const TTableIndex origin );
	inline bool matchLeftDuplicateSaveToTable_WV( const TTableIndex origin );
	inline bool matchRightDuplicate_WV( const TTableIndex origin );
	inline bool matchRightDuplicateSaveToTable_WV( const TTableIndex origin );
	// matching duplicate of E-variables
	inline bool matchLeftDuplicate_E( const TTableIndex origin );
	inline bool matchLeftDuplicateSaveToTable_E( const TTableIndex origin );
	inline bool matchRightDuplicate_E( const TTableIndex origin );
	inline bool matchRightDuplicateSaveToTable_E( const TTableIndex origin );
	// matching closed V-variables
	inline bool matchClosed_V();
	inline bool matchClosedSaveToTable_V();
	inline bool matchClosedWithQualifier_V( const TQualifierIndex qualifier );
	inline bool matchClosedWithQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	// matching closed E-variables
	inline void matchClosed_E();
	inline void matchClosedSaveToTable_E();
	inline bool matchClosedWithQualifier_E( const TQualifierIndex qualifier );
	inline bool matchClosedWithQualifierSaveToTable_E(
		const TQualifierIndex qualifier);
	// matching V-variables by qualifier
	inline bool macthLeftMaxByQualifier_V( const TQualifierIndex qualifier );
	inline bool macthLeftMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	inline bool macthRightMaxByQualifier_V( const TQualifierIndex qualifier );
	inline bool macthRightMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	// matching E-variables by qualifier
	inline void macthLeftMaxByQualifier_E( const TQualifierIndex qualifier );
	inline void macthLeftMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifier_E( const TQualifierIndex qualifier );
	inline void macthRightMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	// match left VE-variable
	inline void matchLeftBegin_E();
	inline void matchLeftBeginSaveToTable_E();
	inline bool matchLeftBegin_V();
	inline bool matchLeftBeginSaveToTable_V();
	inline bool matchLeftWithQulifierBegin_V( const TQualifierIndex qualifier );
	inline bool matchLeftWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	inline bool matchLeft_E();
	inline bool matchLeftSaveToTable_E();
	inline bool matchLeftWithQulifier_E( const TQualifierIndex qualifier );
	inline bool matchLeftWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier );
	// match right VE-variable
	inline void matchRightBegin_E();
	inline void matchRightBeginSaveToTable_E();
	inline bool matchRightBegin_V();
	inline bool matchRightBeginSaveToTable_V();
	inline bool matchRightWithQulifierBegin_V(
		const TQualifierIndex qualifier );
	inline bool matchRightWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	inline bool matchRight_E();
	inline bool matchRightSaveToTable_E();
	inline bool matchRightWithQulifier_E( const TQualifierIndex qualifier );
	inline bool matchRightWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier );
	// making operations
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
	
	TExecutionResult executionResult;
	
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
	
	CUnitNode initialLeftBracket;
	CUnitNode* lastAddedLeftParen;
	CUnitNode* lastAddedLeftBracket;
};

inline void COperationsExecuter::nextOperation()
{
	operation = operation->Next();
	//if( operation->type == OT_Goto ) {
		// TODO: calc new pointer */
	//}
}

inline bool COperationsExecuter::checkQualifier( CUnitNode* const node,
	const TQualifierIndex qualifier ) const
{
	return qualifier->Check( *node );
}

inline void COperationsExecuter::saveToTable( CUnitNode* const node )
{
	table[tableTop] = node;
	tableTop++;
}

inline void COperationsExecuter::saveToTable( CUnitNode* const nodeA,
	CUnitNode* const nodeB )
{
	saveToTable( nodeA );
	saveToTable( nodeB );
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

inline void COperationsExecuter::saveState( COperationNode* operationForSaving )
{
	stack[stackTop].left = left;
	stack[stackTop].right = right;
	stack[stackTop].tableTop = tableTop;
	stack[stackTop].operation = operationForSaving;
	stackTop++;
}

inline bool COperationsExecuter::fail()
{
	if( stackTop > 0 ) {
		stackTop--;
		left = stack[stackTop].left;
		right = stack[stackTop].right;
		tableTop = stack[stackTop].tableTop;
		operation = stack[stackTop].operation;
		return true;
	} else {
		return false;
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

inline void COperationsExecuter::decrementStackDepth( const TUint32 count )
{
	stackTop -= count;
	assert( stackTop >= 0 );
}

inline void COperationsExecuter::setLeftBorder(const TTableIndex tableIndex)
{
	left = table[tableIndex];
}

inline void COperationsExecuter::setRightBorder(const TTableIndex tableIndex)
{
	right = table[tableIndex];
}

inline bool COperationsExecuter::matchEmptyExpression()
{
	return isEmpty();
}

inline bool COperationsExecuter::matchLeftChar( const TChar c )
{
	return ( shiftLeft() && left->IsChar() && left->Char() == c );
}

inline bool COperationsExecuter::matchLeftLabel( const TLabel label )
{
	return ( shiftLeft() && left->IsLabel() && left->Label() == label );
}

inline bool COperationsExecuter::matchLeftNumber( const TNumber number )
{
	return ( shiftLeft() && left->IsNumber() && left->Number() == number );
}

inline bool COperationsExecuter::matchRightChar( const TChar c )
{
	return ( shiftRight() && left->IsChar() && left->Char() == c );
}

inline bool COperationsExecuter::matchRightLabel( const TLabel label )
{
	return ( shiftRight() && left->IsLabel() && left->Label() == label );
}

inline bool COperationsExecuter::matchRightNumber( const TNumber number )
{
	return ( shiftRight() && left->IsNumber() && left->Number() == number );
}

inline bool COperationsExecuter::matchLeftParens()
{
	if( shiftLeft() && left->IsLeftParen() ) {
		right = left->PairedParen();
		saveToTable( left, right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightParens()
{
	if( shiftRight() && right->IsRightParen() ) {
		left = right->PairedParen();
		saveToTable( left, right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeft_S()
{
	return ( shiftLeft() && left->IsSymbol() );
}

inline bool COperationsExecuter::matchLeftSaveToTable_S()
{
	if( shiftLeft() && left->IsSymbol() ) {
		saveToTable( left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQualifier_S(
	const TQualifierIndex qualifier )
{
	return ( shiftLeft() && left->IsSymbol()
		&& checkQualifier( left, qualifier ) );
}

inline bool COperationsExecuter::matchLeftWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && left->IsSymbol() && checkQualifier( left, qualifier ) ) {
		saveToTable( left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRight_S()
{
	return ( shiftRight() && right->IsSymbol() );
}

inline bool COperationsExecuter::matchRightSaveToTable_S()
{
	if( shiftRight() && right->IsSymbol() ) {
		saveToTable( right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQualifier_S(
	const TQualifierIndex qualifier )
{
	return ( shiftRight() && right->IsSymbol()
		&& checkQualifier( right, qualifier ) );
}

inline bool COperationsExecuter::matchRightWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && right->IsSymbol()
		&& checkQualifier( right, qualifier ) )
	{
		saveToTable( right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftDuplicate_S(
	const TTableIndex origin )
{
	return ( shiftLeft() && left->IsEqualWith( *table[origin] ) );
}

inline bool COperationsExecuter::matchLeftDuplicateSaveToTable_S(
	const TTableIndex origin )
{
	if( shiftLeft() && left->IsEqualWith( *table[origin] ) ) {
		saveToTable( left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightDuplicate_S(const TTableIndex origin)
{
	return ( shiftRight() && right->IsEqualWith( *table[origin] ) );
}

inline bool COperationsExecuter::matchRightDuplicateSaveToTable_S(
	const TTableIndex origin )
{
	if( shiftRight() && right->IsEqualWith( *table[origin] ) ) {
		saveToTable( right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeft_W()
{
	if( !shiftLeft() ) {
		return false;
	}
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	return true;
}

inline bool COperationsExecuter::matchLeftSaveToTable_W()
{
	if( !shiftLeft() ) {
		return false;
	}
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	saveToTable( left );
	return true;
}

inline bool COperationsExecuter::matchLeftWithQualifier_W(
	const TQualifierIndex qualifier )
{
	if( !shiftLeft() || !checkQualifier( left, qualifier ) ) {
		return false;
	}
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	return true;
}

inline bool COperationsExecuter::matchLeftWithQualifierSaveToTable_W(
	const TQualifierIndex qualifier )
{
	if( !shiftLeft() || !checkQualifier( left, qualifier ) ) {
		return false;
	}
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
	saveToTable( left );
	return true;
}

inline bool COperationsExecuter::matchRight_W()
{
	if( !shiftRight() ) {
		return false;
	}
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	return true;
}

inline bool COperationsExecuter::matchRightSaveToTable_W()
{
	if( !shiftRight() ) {
		return false;
	}
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right );
	return true;
}

inline bool COperationsExecuter::matchRightWithQualifier_W(
	const TQualifierIndex qualifier )
{
	if( !shiftRight() || !checkQualifier( right, qualifier ) ) {
		return false;
	}
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	return true;
}

inline bool COperationsExecuter::matchRightWithQualifierSaveToTable_W(
	const TQualifierIndex qualifier )
{
	if( !shiftRight() || !checkQualifier( right, qualifier ) ) {
		return false;
	}
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right );
	return true;
}

inline bool COperationsExecuter::matchLeftDuplicate_WV(
	const TTableIndex origin )
{
	CUnitNode* originLeft = table[origin];
	CUnitNode* const originRight = table[origin + 1];
	for( ; originLeft != originRight->Next();
		originLeft = originLeft->Next() )
	{
		if( !shiftLeft() || !left->IsEqualWith( *originLeft ) ) {
			return false;
		}
	}
	return true;
}

inline bool COperationsExecuter::matchLeftDuplicateSaveToTable_WV(
	const TTableIndex origin )
{
	CUnitNode* originLeft = table[origin];
	CUnitNode* const originRight = table[origin + 1];
	CUnitNode* valueBegin = left;
	for( ; originLeft != originRight->Next();
		originLeft = originLeft->Next() )
	{
		if( !shiftLeft() || !left->IsEqualWith( *originLeft ) ) {
			return false;
		}
	}
	saveToTable( valueBegin->Next(), left );
	return true;
}

inline bool COperationsExecuter::matchRightDuplicate_WV(
	const TTableIndex origin )
{
	CUnitNode* const originLeft = table[origin];
	CUnitNode* originRight = table[origin + 1];
	for( ; originRight != originLeft->Prev();
		originRight = originRight->Prev() )
	{
		if( !shiftRight() || !right->IsEqualWith( *originRight ) ) {
			return false;
		}
	}
	return true;
}

inline bool COperationsExecuter::matchRightDuplicateSaveToTable_WV(
	const TTableIndex origin )
{
	CUnitNode* const originLeft = table[origin];
	CUnitNode* originRight = table[origin + 1];
	CUnitNode* valueEnd = right;
	for( ; originRight != originLeft->Prev();
		originRight = originRight->Prev() )
	{
		if( !shiftRight() || !right->IsEqualWith( *originRight ) ) {
			return false;
		}
	}
	saveToTable( right, valueEnd->Prev() );
	return true;
}

inline bool COperationsExecuter::matchLeftDuplicate_E(
	const TTableIndex origin )
{
	if( table[origin] == 0 ) {
		assert( table[origin + 1] == 0 );
		return true;
	} else {
		return matchLeftDuplicate_WV( origin );
	}
}

inline bool COperationsExecuter::matchLeftDuplicateSaveToTable_E(
	const TTableIndex origin )
{
	if( table[origin] == 0 ) {
		assert( table[origin + 1] == 0 );
		return true;
	} else {
		return matchLeftDuplicateSaveToTable_WV( origin );
	}
}

inline bool COperationsExecuter::matchRightDuplicate_E(
	const TTableIndex origin )
{
	if( table[origin] == 0 ) {
		assert( table[origin + 1] == 0 );
		return true;
	} else {
		return matchRightDuplicate_WV( origin );
	}
}

inline bool COperationsExecuter::matchRightDuplicateSaveToTable_E(
	const TTableIndex origin )
{
	if( table[origin] == 0 ) {
		assert( table[origin + 1] == 0 );
		return true;
	} else {
		return matchRightDuplicateSaveToTable_WV( origin );
	}
}

inline bool COperationsExecuter::matchClosed_V()
{
	if( !isEmpty() ) {
		matchClosed_E();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchClosedSaveToTable_V()
{
	saveToTable( left->Next(), right->Prev() );
	return matchClosed_V();
}

inline bool COperationsExecuter::matchClosedWithQualifier_V(
	const TQualifierIndex qualifier )
{
	if( isEmpty() ) {
		return false;
	} else {
		do {
			left = left->Next();
			if( !checkQualifier( left, qualifier ) ) {
				return false;
			}
		} while( left != right->Prev() );
	}
	return true;
}

inline bool COperationsExecuter::matchClosedWithQualifierSaveToTable_V(
	const TQualifierIndex qualifier )
{
	saveToTable( left->Next(), right->Prev() );
	return matchClosedWithQualifier_V( qualifier );
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

inline bool COperationsExecuter::matchClosedWithQualifier_E(
	const TQualifierIndex qualifier )
{
	if( isEmpty() ) {
		return true;
	} else {
		return matchClosedWithQualifier_V( qualifier );
	}
}

inline bool COperationsExecuter::matchClosedWithQualifierSaveToTable_E(
	const TQualifierIndex qualifier )
{
	if( isEmpty() ) {
		saveToTable( 0, 0 );
		return true;
	} else {
		return matchClosedWithQualifierSaveToTable_V( qualifier );
	}
}

inline bool COperationsExecuter::macthLeftMaxByQualifier_V(
	const TQualifierIndex qualifier )
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
	return ( left != first );
}

inline bool COperationsExecuter::macthLeftMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
	if( left != first ) {
		saveToTable( first->Next(), left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::macthRightMaxByQualifier_V(
	const TQualifierIndex qualifier )
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
	return ( right != last );
}

inline bool COperationsExecuter::macthRightMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier )
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
	if( right != last ) {
		saveToTable( right, last->Prev() );
		return true;
	} else {
		return false;
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifier_E(
	const TQualifierIndex qualifier )
{
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier )
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
	const TQualifierIndex qualifier )
{
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
	}
}

inline void COperationsExecuter::macthRightMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier )
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

inline bool COperationsExecuter::matchLeftBegin_V()
{
	if( shiftLeft() ) {
		nextOperation();
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftBeginSaveToTable_V()
{
	if( shiftLeft() ) {
		nextOperation();
		saveState();
		saveToTable( left, left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQulifierBegin_V(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		nextOperation();
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		nextOperation();
		saveState();
		saveToTable( left, left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeft_E()
{
	if( shiftLeft() ) {
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftSaveToTable_E()
{
	if( shiftLeft() ) {
		saveState();
		tableTop++;
		saveToTable( left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQulifier_E(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		saveState();
		tableTop++;
		saveToTable( left );
		return true;
	} else {
		return false;
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

inline bool COperationsExecuter::matchRightBegin_V()
{
	if( shiftRight() ) {
		nextOperation();
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightBeginSaveToTable_V()
{
	if( shiftRight() ) {
		nextOperation();
		saveState();
		saveToTable( right, right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQulifierBegin_V(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		nextOperation();
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		nextOperation();
		saveState();
		saveToTable( right, right );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRight_E()
{
	if( shiftRight() ) {
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightSaveToTable_E()
{
	if( shiftRight() ) {
		saveState();
		saveToTable( right );
		tableTop++;
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQulifier_E(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		saveState();
		saveToTable( right );
		tableTop++;
		return true;
	} else {
		return false;
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
	} else {
		assert( table[tableIndex + 1] == 0 );
	}
}

inline void COperationsExecuter::copy_E( const TTableIndex tableIndex )
{
	if( table[tableIndex] != 0 ) {
		left = fieldOfView.Move( left, table[tableIndex],
			table[tableIndex + 1] );
	} else {
		assert( table[tableIndex + 1] == 0 );
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
