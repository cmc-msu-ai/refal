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

	void Run(COperation* operation, const CFieldOfView& view);
	
private:
	CExecuter(const CExecuter&);
	CExecuter& operator=(const CExecuter&);

	struct CState;

	struct CTemporary {
		CTemporary(CUnitLink* _lb = 0, CUnitLink* _rb = 0,
				COperation* _op = 0, COperation* _next_rule = 0,
				int _table_index = 0, int _stack_depth = 0,
				CUnitLink** _saved_points = 0):
			lb(_lb), first(_lb), rb(_rb), last(_rb),
			op(_op), next_rule(_next_rule),
			table_index(_table_index), stack_depth(_stack_depth),
			saved_points(_saved_points)
		{
		}
		~CTemporary()
		{
			delete[] saved_points;
		}

		inline CTemporary& operator=(const CState& state); 

		CUnitLink* lb;
		CUnitLink* first;
		CUnitLink* rb;
		CUnitLink* last;
		COperation* op;
		COperation* next_rule;
		int table_index;
		int stack_depth;
		CUnitLink** saved_points;
	};

	struct CState {
		CState(CUnitLink* _lb = 0, CUnitLink* _rb = 0,
				COperation* _op = 0, int _table_index = 0):
			lb(_lb), rb(_rb),
			op(_op), table_index(_table_index)
		{
		}
		CState(const CTemporary& t):
			lb(t.lb), rb(t.rb),
			op(t.op), table_index(t.table_index)
		{
		}

		CUnitLink* lb;
		CUnitLink* rb;
		COperation* op;
		int table_index;
	};

	inline void fail(CTemporary& t);
	inline void save(CTemporary& t);
	inline bool shift_left(CTemporary& t);
	inline bool shift_right(CTemporary& t);

	int stack_size;
	CState* stack;
	int table_size;
	CUnitLink** table;
};

inline CExecuter::CTemporary&
CExecuter::CTemporary::operator=(const CState& state)
{
	lb = state.lb;
	rb = state.rb;
	op = state.op;
	table_index = state.table_index;
	return *this;
}

inline void CExecuter::fail(CTemporary& t)
{
	if( t.stack_depth > 0 ) {
		--t.stack_depth;
		t = stack[t.stack_depth];
	} else if( t.next_rule != 0 ) {
		t.lb = t.first;
		t.rb = t.last;
		t.op = t.next_rule;
		t.next_rule = 0;
		t.table_index = 0;
	} else {
		/* TODO: throw fail */
	}
}

inline void CExecuter::save(CTemporary& t)
{
	stack[t.stack_depth] = CState(t);
}

inline bool CExecuter::shift_left(CTemporary& t)
{
	t.lb = t.lb->Next();
	if( t.lb == t.rb ) {
		return false;
	}
	return true;
}

inline bool CExecuter::shift_right(CTemporary& t)
{
	t.rb = t.rb->Prev();
	if( t.lb == t.rb ) {
		return false;
	}
	return true;
}

} // end of namespace refal2
