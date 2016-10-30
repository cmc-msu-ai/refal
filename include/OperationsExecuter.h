#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CExecutionContext

struct CExecutionContext {
	CUnitList Argument;
	const CProgramPtr Program;
	const CReceptaclePtr Receptacle;
	TRuntimeModuleId RuntimeModuleId;

	CExecutionContext( CProgramPtr program, CReceptaclePtr receptacle ) :
		Program( program ),
		Receptacle( receptacle ),
		RuntimeModuleId( InvalidRuntimeModuleId )
	{
		assert( static_cast<bool>( program ) );
		assert( static_cast<bool>( receptacle ) );
	}
};

//-----------------------------------------------------------------------------
// COperationsExecuter

enum TExecutionResult {
	ER_OK = 0,
	ER_RecognitionImpossible,
	ER_CallEmptyFunction,
	ER_LostFunctionLabel,
	ER_WrongArgumentOfExternalFunction
};

class COperationsExecuter : public CExecutionContext {
public:
	static TExecutionResult Run( CProgramPtr program, CReceptaclePtr receptacle,
		CUnitList& fieldOfView,	CUnitNode*& errorCall );

private:
	COperationsExecuter( CProgramPtr program, CReceptaclePtr receptacle );
	~COperationsExecuter();
	COperationsExecuter( const COperationsExecuter& );
	COperationsExecuter& operator=( const COperationsExecuter& );

	void allocateTableAndStack();
	void restoreLeftBrackets();
	void doFunction();
	void doFunctionBody();
	void nextOperation();

	bool checkQualifier( CUnitNode* const node,
		const TQualifierIndex qualifier ) const;

	void saveToTable( CUnitNode* const node );
	void saveToTable( CUnitNode* const nodeA, CUnitNode* const nodeB );

	bool isEmpty() const;

	bool shiftLeft();
	bool shiftRight();

	void saveState( COperationNode* operationForSaving );
	void saveState() { saveState( operation ); }
	bool fail();

	void matchingComplete();
	void doReturn();
	void insertJump( const TOperationAddress operationAddress );
	void decrementStackDepth( const TStackIndex count );
	void setLeftBorder( const TTableIndex tableIndex );
	void setRightBorder( const TTableIndex tableIndex );
	// matching empty expression
	bool matchEmptyExpression();
	// matching symbols
	bool matchLeftChar( const TChar c );
	bool matchLeftLabel( const TLabel label );
	bool matchLeftNumber( const TNumber number );
	bool matchRightChar( const TChar c );
	bool matchRightLabel( const TLabel label );
	bool matchRightNumber( const TNumber number );
	// matching parens
	bool matchLeftParens();
	bool matchRightParens();
	// matching S-variables
	bool matchLeft_S();
	bool matchLeftSaveToTable_S();
	bool matchLeftWithQualifier_S( const TQualifierIndex qualifier );
	bool matchLeftWithQualifierSaveToTable_S( const TQualifierIndex qualifier);
	bool matchRight_S();
	bool matchRightSaveToTable_S();
	bool matchRightWithQualifier_S( const TQualifierIndex qualifier );
	bool matchRightWithQualifierSaveToTable_S( const TQualifierIndex qualifier);
	// matching duplicate of S-variables
	bool matchLeftDuplicate_S( const TTableIndex origin );
	bool matchLeftDuplicateSaveToTable_S( const TTableIndex origin );
	bool matchRightDuplicate_S( const TTableIndex origin );
	bool matchRightDuplicateSaveToTable_S( const TTableIndex origin );
	// matching W-variables
	bool matchLeft_W();
	bool matchLeftSaveToTable_W();
	bool matchLeftWithQualifier_W( const TQualifierIndex qualifier );
	bool matchLeftWithQualifierSaveToTable_W( const TQualifierIndex qualifier );
	bool matchRight_W();
	bool matchRightSaveToTable_W();
	bool matchRightWithQualifier_W( const TQualifierIndex qualifier );
	bool matchRightWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier );
	// matching duplicate of WV-variables
	bool matchLeftDuplicate_WV( const TTableIndex origin );
	bool matchLeftDuplicateSaveToTable_WV( const TTableIndex origin );
	bool matchRightDuplicate_WV( const TTableIndex origin );
	bool matchRightDuplicateSaveToTable_WV( const TTableIndex origin );
	// matching duplicate of E-variables
	bool matchLeftDuplicate_E( const TTableIndex origin );
	bool matchLeftDuplicateSaveToTable_E( const TTableIndex origin );
	bool matchRightDuplicate_E( const TTableIndex origin );
	bool matchRightDuplicateSaveToTable_E( const TTableIndex origin );
	// matching closed V-variables
	bool matchClosed_V();
	bool matchClosedSaveToTable_V();
	bool matchClosedWithQualifier_V( const TQualifierIndex qualifier );
	bool matchClosedWithQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	// matching closed E-variables
	void matchClosed_E();
	void matchClosedSaveToTable_E();
	bool matchClosedWithQualifier_E( const TQualifierIndex qualifier );
	bool matchClosedWithQualifierSaveToTable_E(
		const TQualifierIndex qualifier);
	// matching V-variables by qualifier
	bool macthLeftMaxByQualifier_V( const TQualifierIndex qualifier );
	bool macthLeftMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	bool macthRightMaxByQualifier_V( const TQualifierIndex qualifier );
	bool macthRightMaxByQualifierSaveToTable_V(
		const TQualifierIndex qualifier );
	// matching E-variables by qualifier
	void macthLeftMaxByQualifier_E( const TQualifierIndex qualifier );
	void macthLeftMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	void macthRightMaxByQualifier_E( const TQualifierIndex qualifier );
	void macthRightMaxByQualifierSaveToTable_E(
		const TQualifierIndex qualifier );
	// match left VE-variable
	void matchLeftBegin_E();
	void matchLeftBeginSaveToTable_E();
	bool matchLeftBegin_V();
	bool matchLeftBeginSaveToTable_V();
	bool matchLeftWithQulifierBegin_V( const TQualifierIndex qualifier );
	bool matchLeftWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	bool matchLeft_E();
	bool matchLeftSaveToTable_E();
	bool matchLeftWithQulifier_E( const TQualifierIndex qualifier );
	bool matchLeftWithQulifierSaveToTable_E( const TQualifierIndex qualifier );
	// match right VE-variable
	void matchRightBegin_E();
	void matchRightBeginSaveToTable_E();
	bool matchRightBegin_V();
	bool matchRightBeginSaveToTable_V();
	bool matchRightWithQulifierBegin_V(
		const TQualifierIndex qualifier );
	bool matchRightWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier );
	bool matchRight_E();
	bool matchRightSaveToTable_E();
	bool matchRightWithQulifier_E( const TQualifierIndex qualifier );
	bool matchRightWithQulifierSaveToTable_E( const TQualifierIndex qualifier );
	// making operations
	void insertChar( const TChar c );
	void insertLabel( const TLabel label );
	void insertNumber( const TNumber number );
	void insertLeftParen();
	void insertRightParen();
	void insertRightBracket();
	void move_S( const TTableIndex tableIndex );
	void copy_S( const TTableIndex tableIndex );
	void move_E( const TTableIndex tableIndex );
	void copy_E( const TTableIndex tableIndex );
	void move_WV( const TTableIndex tableIndex );
	void copy_WV( const TTableIndex tableIndex );

	TExecutionResult executionResult;

	CUnitList fieldOfView;
	CUnitNode* left;
	CUnitNode* right;

	CUnitNode** table;
	TTableIndex tableTop;

	COperationNode* operation;

	struct CStackData {
		CUnitNode* left;
		CUnitNode* right;
		TTableIndex tableTop;
		COperationNode* operation;
	};
	CStackData* stack;
	int stackTop;

	CUnitNode initialLeftBracket;
	CUnitNode* lastAddedLeftParen;
	CUnitNode* lastAddedLeftBracket;
};

//-----------------------------------------------------------------------------

const TLabel LabelMask = 256 * 256;

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
	if( node->IsLabel() ) {
		if( ( node->Label() / LabelMask ) != RuntimeModuleId ) {
			return qualifier->IsIncludeAllLabels();
		}
		CUnit unit( *node );
		unit.Label() %= LabelMask;
		return qualifier->Check( unit );
	}
	return qualifier->Check( *node );
}

inline void COperationsExecuter::saveToTable( CUnitNode* const node )
{
	assert( tableTop < Program->MaxTableSize() );
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
	assert( left != nullptr && right != nullptr );
	return ( left->Next() == right );
}

inline bool COperationsExecuter::shiftLeft()
{
	left = left->Next();
	assert( left != nullptr );
	return ( left != right );
}

inline bool COperationsExecuter::shiftRight()
{
	right = right->Prev();
	assert( right != nullptr );
	return ( left != right );
}

inline void COperationsExecuter::saveState( COperationNode* operationForSaving )
{
	assert( stackTop < Program->MaxStackDepth() );
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

inline void COperationsExecuter::decrementStackDepth( const TStackIndex count )
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
	return ( shiftRight() && right->IsChar() && right->Char() == c );
}

inline bool COperationsExecuter::matchRightLabel( const TLabel label )
{
	return ( shiftRight() && right->IsLabel() && right->Label() == label );
}

inline bool COperationsExecuter::matchRightNumber( const TNumber number )
{
	return ( shiftRight() && right->IsNumber() && right->Number() == number );
}

inline bool COperationsExecuter::matchLeftParens()
{
	if( shiftLeft() && left->IsLeftParen() ) {
		saveToTable( left->PairedParen(), right );
		right = left->PairedParen();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightParens()
{
	if( shiftRight() && right->IsRightParen() ) {
		saveToTable( left, right->PairedParen() );
		left = right->PairedParen();
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
	CUnitNode* const tmp = right;
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right, tmp );
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
	CUnitNode* const tmp = right;
	if( right->IsParen() ) {
		right = right->PairedParen();
	}
	saveToTable( right, tmp );
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
			if( left->IsLeftParen() ) {
				left = left->PairedParen();
			}
		} while( !isEmpty() );
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
	left = right->Prev();
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
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
	}
	return ( left != first );
}

inline bool COperationsExecuter::macthLeftMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
	}
	return ( right != last );
}

inline bool COperationsExecuter::macthRightMaxByQualifierSaveToTable_V(
	const TQualifierIndex qualifier )
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
	}
}

inline void COperationsExecuter::macthLeftMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier )
{
	CUnitNode* first = left;
	while( !isEmpty() && checkQualifier( left->Next(), qualifier ) ) {
		left = left->Next();
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
	}
}

inline void COperationsExecuter::macthRightMaxByQualifierSaveToTable_E(
	const TQualifierIndex qualifier )
{
	CUnitNode* last = right;
	while( !isEmpty() && checkQualifier( right->Prev(), qualifier ) ) {
		right = right->Prev();
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		CUnitNode* tmp = left;
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
		nextOperation();
		saveState();
		saveToTable( tmp, left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftWithQulifierBegin_V(
	const TQualifierIndex qualifier )
{
	if( shiftLeft() && checkQualifier( left, qualifier ) ) {
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		CUnitNode* tmp = left;
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
		nextOperation();
		saveState();
		saveToTable( tmp, left );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeft_E()
{
	if( shiftLeft() ) {
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchLeftSaveToTable_E()
{
	if( shiftLeft() ) {
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		if( left->IsLeftParen() ) {
			left = left->PairedParen();
		}
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
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		CUnitNode* tmp = right;
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
		nextOperation();
		saveState();
		saveToTable( right, tmp );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightWithQulifierBegin_V(
	const TQualifierIndex qualifier )
{
	if( shiftRight() && checkQualifier( right, qualifier ) ) {
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		CUnitNode* tmp = right;
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
		nextOperation();
		saveState();
		saveToTable( right, tmp );
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRight_E()
{
	if( shiftRight() ) {
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
		saveState();
		return true;
	} else {
		return false;
	}
}

inline bool COperationsExecuter::matchRightSaveToTable_E()
{
	if( shiftRight() ) {
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
		if( right->IsRightParen() ) {
			right = right->PairedParen();
		}
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
	left->Label() = label + LabelMask * RuntimeModuleId;
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
		left = fieldOfView.Copy( left, table[tableIndex],
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
	left = fieldOfView.Copy( left, table[tableIndex], table[tableIndex + 1] );
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
