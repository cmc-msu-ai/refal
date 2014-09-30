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

	inline void fail();
	inline void save();
	inline bool shift_left();
	inline bool shift_right();

	int stack_size;
	CState* stack;
	int table_size;
	CUnitLink** table;

	CUnitLink* lb;
	CUnitLink* rb;
	COperation* op;
	COperation* next_rule;
	int table_index;
	int stack_depth;
};

inline void CExecuter::fail()
{
	if( stack_depth > 0 ) {
		--stack_depth;
		lb = stack[stack_depth].lb;
		rb = stack[stack_depth].rb;
		op = stack[stack_depth].op;
		table_index = stack[stack_depth].table_index;
	} else if( next_rule != 0 ) {
		op = next_rule;
		next_rule = 0;
		table_index = 0;
	} else {
		/* TODO: throw fail */
	}
}

inline void CExecuter::save()
{
	stack[stack_depth].lb = lb;
	stack[stack_depth].rb = rb;
	stack[stack_depth].op = op;
	stack[stack_depth].table_index = table_index;
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
