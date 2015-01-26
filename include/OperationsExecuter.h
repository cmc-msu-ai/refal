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

	void shiftLeft();
	void shiftRight();
	void fail();

	void matchEmptyExpression();
	void checkNotEmpty();
	void matchLeftChar(const TChar c);
	void matchLeftLabel(const TLabel label);
	void matchLeftNumber(const TNumber number);
	void matchRightChar(const TChar c);
	void matchRightLabel(const TLabel label);
	void matchRightNumber(const TNumber number);

	CUnitList fieldOfView;
	TUnitNode* left;
	TUnitNode* right;

	TUnitNode** table;
	TTableIndex tableTop;
};

} // end of namespace refal2
