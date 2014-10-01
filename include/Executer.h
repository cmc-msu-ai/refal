#pragma once

#include "FieldOfView.h"
#include "Operation.h"

namespace Refal2 {

class CExecuter {
public:
	CExecuter();
	~CExecuter();

	void SetStackSize(int new_stack_size);
	void SetTableSize(int new_table_size);

	void Run(COperation* operation, CUnitLink* first, CUnitLink* last);
	
private:
	CExecuter(const CExecuter&);
	CExecuter& operator=(const CExecuter&);

	struct CState {
		CUnitLink* lb;
		CUnitLink* rb;
		COperation* op;
		int table_index;
	};

	struct CMove {
		CUnitLink* location;
		COperation* op;
	};

	inline void fail();
	inline void save();
	inline bool shift_left();
	inline bool shift_right();

	int stack_size;
	void* stack;
	int table_size;
	CUnitLink** table;

	CUnitLink* lb;
	CUnitLink* rb;
	COperation* op;
	int stack_depth;
	int table_index;

	CUnitLink master_term;
};

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
		/* TODO: throw fail */
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

} // end of namespace refal2
