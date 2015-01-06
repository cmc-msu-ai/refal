#pragma once

#include <Refal2.h>

namespace Refal2 {

struct COperation {
	enum TOperationType {
		OT_goto, /* one argument operation pointer */
		OT_insert_jump, /* one argument operation pointer */
		OT_matching_complete, /* no arguments */

		/* left part operations */
		OT_empty_expression_match, /* no arguments */
		OT_left_symbol_match, /* one argument symbol */
		OT_right_symbol_match, /* one argument symbol */
		OT_left_parens_match, /* no arguments */
		OT_right_parens_match, /* no arguments */
		OT_set_pointers, /* two arguments offset in table */
		OT_left_s_variable_match, /* no arguments */
		OT_right_s_variable_match, /* no arguments */
		OT_left_duplicate_s_variable_match, /* one argument offset in table */
		OT_right_duplicate_s_variable_match, /* one argument offset in table */
		OT_closed_e_variable_match, /* no arguments */
		OT_left_duplicate_wve_variable_match, /* one argument offset in table */
		OT_right_duplicate_wve_variable_match, /* one argument offset in table*/
		OT_left_w_variable_match, /* no arguments */
		OT_right_w_variable_match, /* no arguments */
		OT_check_not_empty, /* no arguments */
		OT_left_e_variable_match_begin, /* no arguments */
		OT_left_v_variable_match_begin, /* no arguments */
		OT_left_ve_variable_match, /* no arguments */
		OT_left_ve_variable_with_qualifier_match, /* one argument qualifier */
		OT_right_e_variable_match_begin, /* no arguments */
		OT_right_v_variable_match_begin, /* no arguments */
		OT_right_ve_variable_match, /* no arguments */
		OT_right_ve_variable_with_qualifier_match, /* one argument qualifier */
		OT_decriase_stack_depth, /* one argument int */
		OT_check_qualifier_last_s_variable, /* one argument qualifier */
		OT_check_qualifier_last_wve_variable, /* one argument qualifier */
		OT_left_max_qualifier, /* one argument qualifier */
		OT_right_max_qualifier, /* one argument qualifier */
		
		/* right part operations */
		OT_insert_symbol, /* one argument symbol */
		OT_insert_left_paren, /* no arguments */
		OT_insert_right_paren, /* no arguments */
		OT_insert_right_bracket, /* no arguments */
		OT_move_s, /* one argument offset in table */
		OT_copy_s, /* one argument offset in table */
		OT_move_e, /* one argument offset in table */
		OT_move_wv, /* one argument offset in table */
		OT_copy_wve, /* one argument offset in table */
		OT_return, /* one argument points array size */
	};
	
	COperation() {}
	COperation(TOperationType _type): type(_type) {}
	
	TOperationType Type() const { return static_cast<TOperationType>(type); }

	bool Is(TOperationType _type) { return ( type == _type ); }

	inline static void Next(COperation*& operation);

	inline COperationOperation* Operation();
	inline const COperationOperation* Operation() const;

	inline COperationInt* Int();
	inline const COperationInt* Int() const;

	inline COperationIntInt* IntInt();
	inline const COperationIntInt* IntInt() const;

	inline COperationUnit* Unit();
	inline const COperationUnit* Unit() const;

	inline COperationQualifier* Qualifier();
	inline const COperationQualifier* Qualifier() const;

	COperation* next;
	char type;

};

struct COperationOperation : public COperation {
	COperationOperation(TOperationType type, COperation* _operation):
		COperation(type), operation(_operation) {}
	
	//inline static void Next(COperation*& operation);

	COperation* operation;
};

struct COperationInt : public COperation {
	COperationInt() {}
	COperationInt(TOperationType type): COperation(type) {}
	COperationInt(int _x): x(_x) {}
	COperationInt(TOperationType type, int _x): COperation(type), x(_x) {}

	//inline static void Next(COperation*& operation);

	int x;
};

struct COperationIntInt : public COperation {
	COperationIntInt() {}
	COperationIntInt(TOperationType type): COperation(type) {}
	COperationIntInt(int _x, int _y): x(_x), y(_y) {}
	COperationIntInt(TOperationType type, int _x, int _y):
		COperation(type), x(_x), y(_y) {}

	//inline static void Next(COperation*& operation);

	int x;
	int y;
};

struct COperationUnit : public COperation {
	COperationUnit(const CUnit& _value): value(_value) {}
	COperationUnit(TOperationType type, const CUnit& _value):
		COperation(type), value(_value) {}

	//inline static void Next(COperation*& operation);
	
	CUnit value;
};

struct COperationQualifier : public COperation {
	COperationQualifier() {}
	COperationQualifier(TOperationType type, const CQualifier* _qualifier):
		COperation(type), qualifier(_qualifier) {}

	//inline static void Next(COperation*& operation);
	
	const CQualifier* qualifier;
};

inline void COperation::Next(COperation*& operation)
{
	//++operation;
	operation = operation->next;
}
/*
inline void COperationOperation::Next(COperation*& operation)
{
	operation = static_cast<COperationOperation*>(operation) + 1;
}

inline void COperationInt::Next(COperation*& operation)
{
	operation = static_cast<COperationInt*>(operation) + 1;
}

inline void COperationIntInt::Next(COperation*& operation)
{
	operation = static_cast<COperationIntInt*>(operation) + 1;
}

inline void COperationUnit::Next(COperation*& operation)
{
	operation = static_cast<COperationUnit*>(operation) + 1;
}
*/
inline COperationOperation* COperation::Operation()
{
	return static_cast<COperationOperation*>(this);
}

inline const COperationOperation* COperation::Operation() const
{
	return static_cast<const COperationOperation*>(this);
}

inline COperationInt* COperation::Int()
{
	return static_cast<COperationInt*>(this);
}

inline const COperationInt* COperation::Int() const
{
	return static_cast<const COperationInt*>(this);
}

inline COperationIntInt* COperation::IntInt()
{
	return static_cast<COperationIntInt*>(this);
}

inline const COperationIntInt* COperation::IntInt() const
{
	return static_cast<const COperationIntInt*>(this);
}

inline COperationUnit* COperation::Unit()
{
	return static_cast<COperationUnit*>(this);
}

inline const COperationUnit* COperation::Unit() const
{
	return static_cast<const COperationUnit*>(this);
}

inline COperationQualifier* COperation::Qualifier()
{
	return static_cast<COperationQualifier*>(this);
}

inline const COperationQualifier* COperation::Qualifier() const
{
	return static_cast<const COperationQualifier*>(this);
}

} // end of namespace refal2
