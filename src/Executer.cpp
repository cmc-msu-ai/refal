#include "Executer.h"

#include "FieldOfView.h"
#include "Operation.h"
#include "Qualifier.h"
#include "Common.h"

namespace Refal2 {

CExecuter::CExecuter():
	stack_size(0), stack(0), table_size(0), table(0)
{
}

CExecuter::~CExecuter()
{
	delete[] stack;
	delete[] table;
}

void CExecuter::SetStackSize(int new_stack_size)
{
	if( new_stack_size > stack_size ) {
		delete[] stack;
		stack_size = new_stack_size;
		stack = new CState[stack_size];
	}
}

void CExecuter::SetTableSize(int new_table_size)
{
	if( new_table_size > table_size ) {
		delete[] table;
		table_size = new_table_size;
		table = new CUnitLink*[table_size];
	}
}

const char* names[] = {
	"OT_goto", /* one argument operation pointer */
	"OT_set_next_rule", /* one argument operation pointer */
	"OT_matching_complete", /* no arguments */
	"OT_empty_expression_match", /* no arguments */
	"OT_left_symbol_match", /* one argument symbol */
	"OT_right_symbol_match", /* one argument symbol */
	"OT_left_parens_match", /* no arguments */
	"OT_right_parens_match", /* no arguments */
	"OT_set_pointers", /* two arguments offset in table */
	"OT_left_s_variable_match", /* no arguments */
	"OT_right_s_variable_match", /* no arguments */
	"OT_left_duplicate_s_variable_match", /* one argument offset in table */
	"OT_right_duplicate_s_variable_match", /* one argument offset in table */
	"OT_closed_e_variable_match", /* no arguments */
	"OT_left_duplicate_wve_variable_match", /* one argument offset in table */
	"OT_right_duplicate_wve_variable_match", /* one argument offset in table*/
	"OT_left_w_variable_match", /* no arguments */
	"OT_right_w_variable_match", /* no arguments */
	"OT_check_not_empty", /* no arguments */
	"OT_left_e_variable_match_begin", /* no arguments */
	"OT_left_v_variable_match_begin", /* no arguments */
	"OT_left_ve_variable_match", /* no arguments */
	"OT_left_ve_variable_with_qualifier_match", /* one argument qualifier */
	"OT_right_e_variable_match_begin", /* no arguments */
	"OT_right_v_variable_match_begin", /* no arguments */
	"OT_right_ve_variable_match", /* no arguments */
	"OT_right_ve_variable_with_qualifier_match", /* one argument qualifier */
	"OT_decriase_stack_depth", /* one argument int */
	"OT_check_qualifier_last_s_variable", /* one argument qualifier */
	"OT_check_qualifier_last_wve_variable", /* one argument qualifier */
	"OT_left_max_qualifier", /* one argument qualifier */
	"OT_right_max_qualifier", /* one argument qualifier */
	"OT_insert_symbol", /* one argument symbol */
	"OT_insert_parens", /* no arguments */
	"OT_jump_right_paren", /* no arguments */
	"OT_save_point", /* one argument offset in points array */
	"OT_allocate_points_array", /* one argument points array size */
	"OT_move_s", /* one argument offset in table */
	"OT_copy_s", /* one argument offset in table */
	"OT_move_wve", /* one argument offset in table */
	"OT_copy_wve", /* one argument offset in table */
	"OT_return" /* one argument points array size */
};

void print(COperation* operation)
{
	std::cout << names[operation->Type()] << "\n";
}

void CExecuter::Run(COperation* operation, const CFieldOfView& view)
{
	CTemporary t(view.First(), view.Last(), operation);	
	try {
		while( true ) {
			print(t.op);
			switch( t.op->Type() ) {
				case COperation::OT_goto:
					t.op = t.op->Operation()->operation;
					break;
					
				case COperation::OT_set_next_rule:
					t.next_rule = t.op->Operation()->operation;
					COperationOperation::Next(t.op);
					break;

				case COperation::OT_matching_complete:
					t.lb = t.first;
					{
						CFieldOfView tmp;
						CFieldOfView::Move(tmp.First(),
							t.first->Next(), t.last->Prev());
						t.first = tmp.First();
						t.last = tmp.Last();
					}
					COperation::Next(t.op);
					break;

				/* left part operations */
				case COperation::OT_empty_expression_match:
					if( t.lb->Next() == t.rb ) {
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_left_symbol_match:
					
					if( shift_left(t) && *t.lb == t.op->Unit()->value ) {
						table[t.table_index] = t.lb;
						++t.table_index;
						COperationUnit::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_symbol_match:
					if( shift_right(t) && *t.rb == t.op->Unit()->value ) {
						table[t.table_index] = t.rb;
						++t.table_index;
						COperationUnit::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_left_parens_match:
					if( shift_left(t) && t.lb->IsLeftParen() ) {
						t.rb = t.lb->PairedParen();
						table[t.table_index] = t.lb;
						++t.table_index;
						table[t.table_index] = t.rb;
						++t.table_index;
						COperation::Next(t.op);
					} else {
						fail(t);
					}				
					break;

				case COperation::OT_right_parens_match:
					if( shift_right(t) && t.rb->IsRightParen() ) {
						t.lb = t.rb->PairedParen();
						table[t.table_index] = t.lb;
						++t.table_index;
						table[t.table_index] = t.rb;
						++t.table_index;
						COperation::Next(t.op);
					} else {
						fail(t);
					}				
					break;

				case COperation::OT_set_pointers:
					t.lb = table[t.op->IntInt()->x];
					t.rb = table[t.op->IntInt()->y];
					COperationIntInt::Next(t.op);
					break;

				case COperation::OT_left_s_variable_match:
					if( shift_left(t) && t.lb->IsSymbol() ) {
						table[t.table_index] = t.lb;
						++t.table_index;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_s_variable_match:
					if( shift_right(t) && t.rb->IsSymbol() ) {
						table[t.table_index] = t.rb;
						++t.table_index;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_left_duplicate_s_variable_match:
					if( shift_left(t) && *t.lb == *table[t.op->Int()->x] )
					{
						table[t.table_index] = t.lb;
						++t.table_index;
						COperationInt::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_duplicate_s_variable_match:
					if( shift_right(t) && *t.rb == *table[t.op->Int()->x] )
					{
						table[t.table_index] = t.rb;
						++t.table_index;
						COperationInt::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_closed_e_variable_match:
					table[t.table_index] = t.lb->Next();
					++t.table_index;
					table[t.table_index] = t.rb->Prev();
					++t.table_index;
					COperation::Next(t.op);
					break;

				case COperation::OT_left_duplicate_wve_variable_match:
					COperationInt::Next(t.op);
					break;

				case COperation::OT_right_duplicate_wve_variable_match:
					COperationInt::Next(t.op);
					break;

				case COperation::OT_left_w_variable_match:
					if( shift_left(t) ) {
						table[t.table_index] = t.lb;
						++t.table_index;
						if( t.lb->IsLeftParen() )	{
							t.lb = t.lb->PairedParen();
						}
						table[t.table_index] = t.lb;
						++t.table_index;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_w_variable_match:
					if( shift_right(t) ) {
						table[t.table_index + 1] = t.rb;
						if( t.rb->IsRightParen() ) 	{
							t.rb = t.rb->PairedParen();
						}
						table[t.table_index] = t.rb;
						t.table_index += 2;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_check_not_empty:
					if( table[t.table_index - 1]->Next() ==
						table[t.table_index - 2] ) {
						fail(t);
					} else {
						COperation::Next(t.op);
					}
					break;

				case COperation::OT_left_e_variable_match_begin:
					save(t);
					++t.stack_depth;
					table[t.table_index] = t.lb->Next();
					++t.table_index;
					table[t.table_index] = t.lb;
					++t.table_index;
					COperation::Next(t.op);
					if( t.op->Is(COperation::OT_left_ve_variable_match) ) {
						COperation::Next(t.op);
					} else {
						COperationQualifier::Next(t.op);
					}
					break;

				case COperation::OT_left_v_variable_match_begin:
					save(t);
					table[t.table_index] = t.lb->Next();
					table[t.table_index + 1] = t.lb;
					COperation::Next(t.op);
					break;

				case COperation::OT_left_ve_variable_match:
					t.lb = table[t.table_index + 1];
					if( shift_left(t) ) {
						if( t.lb->IsLeftParen() ) {
							t.lb = t.lb->PairedParen();
						}
						++t.stack_depth;
						table[t.table_index + 1] = t.lb;
						t.table_index += 2;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_left_ve_variable_with_qualifier_match:
					t.lb = table[t.table_index + 1];
					if( shift_left(t) ) {
						if( t.lb->IsLeftParen() ) {
							t.lb = t.lb->PairedParen();
						}
						/* check qualifier */
						if( t.op->Qualifier()->qualifier->Check(t.lb) ) {
							++t.stack_depth;
							table[t.table_index + 1] = t.lb;
							t.table_index += 2;
							COperationQualifier::Next(t.op);
						} else {
							fail(t);
						}
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_e_variable_match_begin:
					save(t);
					++t.stack_depth;
					table[t.table_index] = t.rb;
					++t.table_index;
					table[t.table_index] = t.rb->Prev();
					++t.table_index;
					COperation::Next(t.op);
					if( t.op->Is(COperation::OT_right_ve_variable_match) ) {
						COperation::Next(t.op);
					} else {
						COperationQualifier::Next(t.op);
					}
					break;

				case COperation::OT_right_v_variable_match_begin:
					save(t);
					table[t.table_index] = t.rb;
					++t.table_index;
					table[t.table_index] = t.rb->Prev();
					++t.table_index;
					COperation::Next(t.op);
					break;

				case COperation::OT_right_ve_variable_match:
					t.rb = table[t.table_index];
					if( shift_right(t) ) {
						if( t.rb->IsRightParen() ) {
							t.rb = t.rb->PairedParen();
						}
						++t.stack_depth;
						table[t.table_index] = t.rb;
						t.table_index += 2;
						COperation::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_right_ve_variable_with_qualifier_match:
					t.rb = table[t.table_index];
					if( shift_right(t) ) {
						if( t.rb->IsRightParen() ) {
							t.rb = t.rb->PairedParen();
						}
						/* check qualifier */
						if( t.op->Qualifier()->qualifier->Check(t.rb) ) {
							++t.stack_depth;
							table[t.table_index] = t.rb;
							t.table_index += 2;
							COperationQualifier::Next(t.op);
						} else {
							fail(t);
						}
					} else {
						fail(t);
					}
					break;

				case COperation::OT_decriase_stack_depth:
					t.stack_depth -= operation->Int()->x;
					COperationInt::Next(t.op);
					break;

				case COperation::OT_check_qualifier_last_s_variable:
					if( t.op->Qualifier()->qualifier->
						Check(table[t.table_index - 1]) )
					{
						COperationQualifier::Next(t.op);
					} else {
						fail(t);
					}
					break;

				case COperation::OT_check_qualifier_last_wve_variable:
					{
						CUnitLink* i = table[t.table_index - 2]->Prev();
						CUnitLink* till = table[t.table_index - 1];
						while( i != till ) {
							i = i->Next();
							if( i->IsLeftParen() ) {
								i = i->PairedParen();
							}
							if( !( t.op->Qualifier()->qualifier->Check(i) ) ) {
								fail(t);
								break;
							}
						}
						if( i == till ) {
							COperationQualifier::Next(t.op);
						}
					}
					break;

				case COperation::OT_left_max_qualifier:
					t.lb = t.lb->Next();
					table[t.table_index] = t.lb;
					++t.table_index;
					for( ; t.lb != t.rb; t.lb = t.lb->Next() ) {
						if( !( t.op->Qualifier()->qualifier->Check(t.lb) ) ) {
							break;
						} else if( t.lb->IsLeftParen() ) {
							t.lb = t.lb->PairedParen();
						}
					}
					t.lb = t.lb->Prev();
					table[t.table_index] = t.lb;
					++t.table_index;
					COperationQualifier::Next(t.op);
					break;

				case COperation::OT_right_max_qualifier:
					t.rb = t.rb->Prev();
					table[t.table_index + 1] = t.lb;
					for( ; t.rb != t.lb; t.rb = t.rb->Prev() ) {
						if( !( t.op->Qualifier()->qualifier->Check(t.rb) ) ) {
							break;
						} else if( t.rb->IsRightParen() ) {
							t.rb = t.rb->PairedParen();
						}
					}
					t.rb = t.rb->Next();
					table[t.table_index] = t.rb;
					t.table_index += 2;
					COperationQualifier::Next(t.op);
					break;

				/* right part operations */
				case COperation::OT_insert_symbol:
					t.lb = view.Insert(t.lb, t.op->Unit()->value);
					COperationUnit::Next(t.op);
					break;

				case COperation::OT_insert_parens:
					{
						CUnitLink* right_paren = view.Insert(t.lb,
							CUnitValue(CLink::T_right_paren));
						CUnitLink* left_paren = view.Insert(t.lb,
							CUnitValue(CLink::T_left_paren));
						right_paren->PairedParen() = left_paren;
						left_paren->PairedParen() = right_paren;
						t.lb = left_paren;
					}
					COperation::Next(t.op);
					break;

				case COperation::OT_jump_right_paren:
					shift_left(t);
					COperation::Next(t.op);
					break;

				case COperation::OT_save_point:
					t.saved_points[t.op->Int()->x] = t.lb;
					COperationInt::Next(t.op);
					break;

				case COperation::OT_allocate_points_array:
					t.saved_points = new CUnitLink*[t.op->Int()->x];
					COperationInt::Next(t.op);
					break;

				case COperation::OT_move_s:
					t.lb =  CFieldOfView::Move(t.lb, table[t.op->Int()->x]);
					COperationInt::Next(t.op);
					break;

				case COperation::OT_copy_s:
					t.lb =  CFieldOfView::Copy(t.lb, table[t.op->Int()->x]);
					COperationInt::Next(t.op);
					break;

				case COperation::OT_move_wve:
					t.lb = CFieldOfView::Move(t.lb, table[t.op->Int()->x - 1],
						table[t.op->Int()->x]);
					COperationInt::Next(t.op);
					break;

				case COperation::OT_copy_wve:
					t.lb = CFieldOfView::Copy(t.lb, table[t.op->Int()->x - 1],
						table[t.op->Int()->x]);
					COperationInt::Next(t.op);
					break;

				case COperation::OT_return:
					CFieldOfView::Remove(t.first, t.last);
					for( int i = 0; i < t.op->Int()->x; i += 2 ) {
						t.saved_points[i] =  t.saved_points[i]->Next();
					}
					for( int i = 0; i < t.op->Int()->x; i += 2 ) {
						CUnitLink* name = t.saved_points[i];
						if( name->IsLabel() ) {
							Run(name->Label()->second.operation, CFieldOfView(
								name, t.saved_points[i + 1]->Next()));
							CFieldOfView::Remove(name);
						} else {
							std::cout << "vse ochen' ploho" << i << "\n";
							view.Print();
							std::cout << "\n\n";
							/* TOWO: throw: trying to call not label */
						}
					}
					return;
					break;

				default:
					/* assert */
					break;
			}
		}
	} catch(...) {
		/* DEBUG INFO */
		throw;
	}
}

} // end of namespace refal2
