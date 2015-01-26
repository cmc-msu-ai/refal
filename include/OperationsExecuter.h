#pragma once

#include <Refal2.h>

namespace Refal2 {

#if 0
class CExecuter {
public:
	CExecuter();
	~CExecuter();

	void SetStackSize(int new_stack_size);
	void SetTableSize(int new_table_size);

	void Run(COperation* operation, TUnitNode* first, TUnitNode* last);
	
private:
	CExecuter(const CExecuter&);
	CExecuter& operator=(const CExecuter&);

	struct CState {
		TUnitNode* lb;
		TUnitNode* rb;
		COperation* op;
		int table_index;
	};

	struct CMove {
		TUnitNode* location;
		COperation* op;
	};

	inline CState& states_stack(int i);
	inline CMove& moves_stack(int i);

	inline void fail();
	inline void save();
	inline bool shift_left();
	inline bool shift_right();

	int stack_size;
	void* stack;
	int table_size;
	TUnitNode** table;

	TUnitNode* lb;
	TUnitNode* rb;
	COperation* op;
	int stack_depth;
	int table_index;

	TUnitNode master_term;
};

inline CExecuter::CState& CExecuter::states_stack(int i)
{
	return static_cast<CState*>(stack)[i];
}

inline CExecuter::CMove& CExecuter::moves_stack(int i)
{
	return static_cast<CMove*>(stack)[i];
}

inline void CExecuter::fail()
{
	if( stack_depth > 0 ) {
		--stack_depth;
		CState* sp = static_cast<CState*>(stack);
		lb = sp[stack_depth].lb;
		rb = sp[stack_depth].rb;
		op = sp[stack_depth].op;
		table_index = sp[stack_depth].table_index;
	} else {
		/* TODO: throw fail recognition impossible */
	}
}

inline void CExecuter::save()
{
	CState* sp = static_cast<CState*>(stack);
	sp[stack_depth].lb = lb;
	sp[stack_depth].rb = rb;
	sp[stack_depth].op = op;
	sp[stack_depth].table_index = table_index;
}

inline bool CExecuter::shift_left()
{
	lb = lb->Next();
	if( lb == rb ) {
		return false;
	}
	return true;
}

inline bool CExecuter::shift_right()
{
	rb = rb->Prev();
	if( lb == rb ) {
		return false;
	}
	return true;
}
#endif

// ---------------------------------------------------------------------------

class COperationsExecuter {
public:
	COperationsExecuter();
	
private:
	COperationsExecuter(const COperationsExecuter&);
	COperationsExecuter& operator=(const COperationsExecuter&);
	
	void doOperation();
	inline void nextOperation();
	
	inline bool checkQualifier(TUnitNode* const node,
		const TQualifierIndex qualifier) const;
	
	inline void saveToTable(TUnitNode* const node);
	inline void saveToTable(TUnitNode* const nodeA, TUnitNode* const nodeB);
	
	inline bool isEmpty() const;
	
	inline void shiftLeft();
	inline void shiftRight();

	inline void saveState();
	inline void fail();
	
	inline void setLeftBorder(const TTableIndex tableIndex);
	inline void setRightBorder(const TTableIndex tableIndex);
	
	inline void checkNotEmpty();
	/* matching empty expression */
	inline void matchEmptyExpression();
	/* matching symbols */
	inline void matchLeftChar(const TChar c);
	inline void matchLeftLabel(const TLabel label);
	inline void matchLeftNumber(const TNumber number);
	inline void matchRightChar(const TChar c);
	inline void matchRightLabel(const TLabel label);
	inline void matchRightNumber(const TNumber number);
	/* matching parens */
	inline void matchLeftParens();
	inline void matchRightParens();
	/* matching S-variables */
	inline void matchLeft_S();
	inline void matchLeftSaveToTable_S();
	inline void matchLeftWithQualifier_S(const TQualifierIndex qualifier);
	inline void matchLeftWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	inline void matchRight_S();
	inline void matchRightSaveToTable_S();
	inline void matchRightWithQualifier_S(const TQualifierIndex qualifier);
	inline void matchRightWithQualifierSaveToTable_S(
		const TQualifierIndex qualifier);
	/* matching duplicate of S-variables */
	inline void matchLeftDuplicate_S(const TTableIndex origin);
	inline void matchLeftDuplicateSaveToTable_S(const TTableIndex origin);
	inline void matchRightDuplicate_S(const TTableIndex origin);
	inline void matchRightDuplicateSaveToTable_S(const TTableIndex origin);
	/* matching W-variables */
	inline void matchLeft_W();
	inline void matchLeftSaveToTable_W();
	inline void matchLeftWithQualifier_W(const TQualifierIndex qualifier);
	inline void matchLeftWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier);
	inline void matchRight_W();
	inline void matchRightSaveToTable_W();
	inline void matchRightWithQualifier_W(const TQualifierIndex qualifier);
	inline void matchRightWithQualifierSaveToTable_W(
		const TQualifierIndex qualifier);
	/* matching duplicate of WV-variables */
	inline void matchLeftDuplicate_WV(const TTableIndex origin);
	inline void matchLeftDuplicateSaveToTable_WV(const TTableIndex origin);
	inline void matchRightDuplicate_WV(const TTableIndex origin);
	inline void matchRightDuplicateSaveToTable_WV(const TTableIndex origin);
	/* matching duplicate of E-variables */
	inline void matchLeftDuplicate_E(const TTableIndex origin);
	inline void matchLeftDuplicateSaveToTable_E(const TTableIndex origin);
	inline void matchRightDuplicate_E(const TTableIndex origin);
	inline void matchRightDuplicateSaveToTable_E(const TTableIndex origin);
	/* matching closed V-variables */
	inline void matchClosed_V();
	inline void matchClosedSaveToTable_V();
	inline void matchClosedWithQualifier_V(const TQualifierIndex qualifier);
	inline void matchClosedWithQualifierSaveToTable_V(
		const TQualifierIndex qualifier);
	/* matching closed E-variables */
	inline void matchClosed_E();
	inline void matchClosedSaveToTable_E();
	inline void matchClosedWithQualifier_E(const TQualifierIndex qualifier);
	inline void matchClosedWithQualifierSaveToTable_E(
		const TQualifierIndex qualifier);
	/* matching by qualifier */
	inline void macthLeftMaxByQualifier(const TQualifierIndex qualifier);
	inline void macthLeftMaxByQualifierSaveToTable(
		const TQualifierIndex qualifier);
	inline void macthRightMaxByQualifier(const TQualifierIndex qualifier);
	inline void macthRightMaxByQualifierSaveToTable(
		const TQualifierIndex qualifier);
	/* match left VE-variable */
	inline void matchLeftBegin_E();
	inline void matchLeftBeginSaveToTable_E();
	inline void matchLeftBegin_V();
	inline void matchLeftBeginSaveToTable_V();
	inline void matchLeftWithQulifierBegin_V(const TQualifierIndex qualifier);
	inline void matchLeftWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier);
	inline void matchLeft_E();
	inline void matchLeftSaveToTable_E();
	inline void matchLeftWithQulifier_E(const TQualifierIndex qualifier);
	inline void matchLeftWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier);
	/* match right VE-variable */
	inline void matchRightBegin_E();
	inline void matchRightBeginSaveToTable_E();
	inline void matchRightBegin_V();
	inline void matchRightBeginSaveToTable_V();
	inline void matchRightWithQulifierBegin_V(const TQualifierIndex qualifier);
	inline void matchRightWithQulifierBeginSaveToTable_V(
		const TQualifierIndex qualifier);
	inline void matchRight_E();
	inline void matchRightSaveToTable_E();
	inline void matchRightWithQulifier_E(const TQualifierIndex qualifier);
	inline void matchRightWithQulifierSaveToTable_E(
		const TQualifierIndex qualifier);
	
	CUnitList fieldOfView;
	TUnitNode* left;
	TUnitNode* right;
	
	TUnitNode** table;
	TTableIndex tableTop;

	COperation* operation;
};

inline void COperationsExecuter::nextOperation()
{
	operation++;
	if( operation->type == OT_Goto ) {
		// TODO: calc new pointer */
	}
}

inline bool COperationsExecuter::checkQualifier(TUnitNode* const node,
	const TQualifierIndex qualifier) const
{
	return true;
}

inline void COperationsExecuter::saveToTable(TUnitNode* const node)
{
	table[tableTop] = node;
	tableTop++;
}

inline void COperationsExecuter::saveToTable(TUnitNode* const nodeA,
	TUnitNode* const nodeB)
{
	saveToTable( nodeA );
	saveToTable( nodeB );
}

inline bool COperationsExecuter::isEmpty() const
{
	return ( left->Next() == right );
}

inline void COperationsExecuter::shiftLeft()
{
	left = left->Next();
	if( left == right ) {
		fail();
	}
}

inline void COperationsExecuter::shiftRight()
{
	right = right->Prev();
	if( left == right ) {
		fail();
	}
}

inline void COperationsExecuter::saveState()
{
	// TODO: save state
}

inline void COperationsExecuter::fail()
{
	// TODO: fail
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

inline void COperationsExecuter::checkNotEmpty()
{
	if( isEmpty() ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftChar(const TChar c)
{
	shiftLeft();
	if( !left->IsChar() || left->Char() != c ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftLabel(const TLabel label)
{
	shiftLeft();
	if( !left->IsLabel() || left->Label() != label ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftNumber(const TNumber number)
{
	shiftLeft();
	if( !left->IsNumber() || left->Number() != number ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightChar(const TChar c)
{
	shiftRight();
	if( !left->IsChar() || left->Char() != c ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightLabel(const TLabel label)
{
	shiftRight();
	if( !left->IsLabel() || left->Label() != label ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightNumber(const TNumber number)
{
	shiftRight();
	if( !left->IsNumber() || left->Number() != number ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftParens()
{
	shiftLeft();
	if( left->IsLeftParen() ) {
		right = left->PairedParen();
		saveToTable( left, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightParens()
{
	shiftRight();
	if( right->IsRightParen() ) {
		left = right->PairedParen();
		saveToTable( left, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_S()
{
	shiftLeft();
	if( !left->IsSymbol() ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftSaveToTable_S()
{
	shiftLeft();
	if( left->IsSymbol() ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQualifier_S(
	const TQualifierIndex qualifier)
{
	shiftLeft();
	if( !left->IsSymbol()  || !checkQualifier( left, qualifier ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier)
{
	shiftLeft();
	if( left->IsSymbol() && checkQualifier( left, qualifier ) ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRight_S()
{
	shiftRight();
	if( !right->IsSymbol() ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightSaveToTable_S()
{
	shiftRight();
	if( right->IsSymbol() ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQualifier_S(
	const TQualifierIndex qualifier)
{
	shiftRight();
	if( !right->IsSymbol() || !checkQualifier( right, qualifier ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQualifierSaveToTable_S(
	const TQualifierIndex qualifier)
{
	shiftRight();
	if( right->IsSymbol() && checkQualifier( right, qualifier ) ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftDuplicate_S(const TTableIndex origin)
{
	shiftLeft();
	if( !CompareUnit( *left, *table[origin] ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftDuplicateSaveToTable_S(
	const TTableIndex origin)
{
	shiftLeft();
	if( CompareUnit( *left, *table[origin] ) ) {
		saveToTable( left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightDuplicate_S(const TTableIndex origin)
{
	shiftRight();
	if( !CompareUnit( *right, *table[origin] ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightDuplicateSaveToTable_S(
	const TTableIndex origin)
{
	shiftRight();
	if( CompareUnit( *right, *table[origin] ) ) {
		saveToTable( right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_W()
{
	shiftLeft();
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
}

inline void COperationsExecuter::matchLeftSaveToTable_W()
{
	shiftLeft();
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
		saveToTable( left );
	}
}

inline void COperationsExecuter::matchLeftWithQualifier_W(
	const TQualifierIndex qualifier)
{
	shiftLeft();
	if( !checkQualifier( left, qualifier ) ) {
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
	shiftLeft();
	if( !checkQualifier( left, qualifier ) ) {
		fail();
		return;
	}
	saveToTable( left );
	if( left->IsParen() ) {
		left = left->PairedParen();
		saveToTable( left );
	}
}

inline void COperationsExecuter::matchRight_W()
{
	shiftRight();
	if( left->IsParen() ) {
		left = left->PairedParen();
	}
}

inline void COperationsExecuter::matchRightSaveToTable_W()
{
	shiftRight();
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
		saveToTable( right );
	}
}

inline void COperationsExecuter::matchRightWithQualifier_W(
	const TQualifierIndex qualifier)
{
	shiftRight();
	if( !checkQualifier( right, qualifier ) ) {
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
	shiftRight();
	if( !checkQualifier( right, qualifier ) ) {
		fail();
		return;
	}
	saveToTable( right );
	if( right->IsParen() ) {
		right = right->PairedParen();
		saveToTable( right );
	}
}

inline void COperationsExecuter::matchLeftDuplicate_WV(
	const TTableIndex origin)
{
	TUnitNode* originLeft = table[origin];
	TUnitNode* const originRight = table[origin + 1];
	
	shiftLeft();
	for(; originLeft != originRight; originLeft = originLeft->Next() ) {
		if( !CompareUnit( *left, *originLeft ) ) {
			fail();
			return;
		}
		shiftLeft();
	}
	if( !CompareUnit( *left, *originLeft ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchLeftDuplicateSaveToTable_WV(
	const TTableIndex origin)
{
	TUnitNode* originLeft = table[origin];
	TUnitNode* const originRight = table[origin + 1];
	
	shiftLeft();
	saveToTable( left );
	for(; originLeft != originRight; originLeft = originLeft->Next() ) {
		if( !CompareUnit( *left, *originLeft ) ) {
			fail();
			return;
		}
		shiftLeft();
	}
	if( !CompareUnit( *left, *originLeft ) ) {
		fail();
	} else {
		saveToTable( left );
	}
}

inline void COperationsExecuter::matchRightDuplicate_WV(
	const TTableIndex origin)
{
	TUnitNode* const originLeft = table[origin];
	TUnitNode* originRight = table[origin + 1];
	
	shiftRight();
	for(; originRight != originLeft; originRight = originRight->Next() ) {
		if( !CompareUnit( *right, *originRight ) ) {
			fail();
			return;
		}
		shiftRight();
	}
	if( !CompareUnit( *right, *originRight ) ) {
		fail();
	}
}

inline void COperationsExecuter::matchRightDuplicateSaveToTable_WV(
	const TTableIndex origin)
{
	TUnitNode* const originLeft = table[origin];
	TUnitNode* originRight = table[origin + 1];
	
	shiftRight();
	saveToTable( right );
	for(; originRight != originLeft; originRight = originRight->Next() ) {
		if( !CompareUnit( *right, *originRight ) ) {
			fail();
			return;
		}
		shiftRight();
	}
	if( !CompareUnit( *right, *originRight ) ) {
		fail();
	} else {
		saveToTable( right );
	}
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
}

inline void COperationsExecuter::matchClosedSaveToTable_V()
{
}

inline void COperationsExecuter::matchClosedWithQualifier_V(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::matchClosedWithQualifierSaveToTable_V(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::matchClosed_E()
{
}

inline void COperationsExecuter::matchClosedSaveToTable_E()
{
}

inline void COperationsExecuter::matchClosedWithQualifier_E(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::matchClosedWithQualifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::macthLeftMaxByQualifier(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::macthLeftMaxByQualifierSaveToTable(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::macthRightMaxByQualifier(
	const TQualifierIndex qualifier)
{
}

inline void COperationsExecuter::macthRightMaxByQualifierSaveToTable(
	const TQualifierIndex qualifier)
{
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
	nextOperation();
	shiftLeft();
	saveState();
}

inline void COperationsExecuter::matchLeftBeginSaveToTable_V()
{
	nextOperation();
	shiftLeft();
	saveState();
	saveToTable( left, left );
}

inline void COperationsExecuter::matchLeftWithQulifierBegin_V(
	const TQualifierIndex qualifier)
{
	nextOperation();
	shiftLeft();
	if( checkQualifier( left, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier)
{
	nextOperation();
	shiftLeft();
	if( checkQualifier( left, qualifier ) ) {
		saveState();
		saveToTable( left, left );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeft_E()
{
	shiftLeft();
	saveState();
}

inline void COperationsExecuter::matchLeftSaveToTable_E()
{
	shiftLeft();
	saveState();
	saveToTable( left );
	tableTop++;
}

inline void COperationsExecuter::matchLeftWithQulifier_E(
	const TQualifierIndex qualifier)
{
	shiftLeft();
	if( checkQualifier( left, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchLeftWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	shiftLeft();
	if( checkQualifier( left, qualifier ) ) {
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
	nextOperation();
	shiftRight();
	saveState();
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
	nextOperation();
	shiftRight();
	if( checkQualifier( right, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQulifierBeginSaveToTable_V(
	const TQualifierIndex qualifier)
{
	nextOperation();
	shiftRight();
	if( checkQualifier( right, qualifier ) ) {
		saveState();
		saveToTable( right, right );
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRight_E()
{
	shiftRight();
	saveState();
}

inline void COperationsExecuter::matchRightSaveToTable_E()
{
	shiftRight();
	saveState();
	tableTop++;
	saveToTable( right );
}

inline void COperationsExecuter::matchRightWithQulifier_E(
	const TQualifierIndex qualifier)
{
	shiftRight();
	if( checkQualifier( right, qualifier ) ) {
		saveState();
	} else {
		fail();
	}
}

inline void COperationsExecuter::matchRightWithQulifierSaveToTable_E(
	const TQualifierIndex qualifier)
{
	shiftRight();
	if( checkQualifier( right, qualifier ) ) {
		saveState();
		tableTop++;
		saveToTable( right );
	} else {
		fail();
	}
}

} // end of namespace refal2
