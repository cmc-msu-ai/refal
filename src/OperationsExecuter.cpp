#include <iostream>
#include <Refal2.h>

namespace Refal2 {

#if 0
CExecuter::CExecuter():
	stack_size(0), stack(0),
	table_size(0), table(0),
	lb(0), rb(0),
	op(0),
	stack_depth(0),
	table_index(0),
	master_term( CUnit(UT_LeftBracket) )
{
}

CExecuter::~CExecuter()
{
	::operator delete(stack);
	delete[] table;
}

void CExecuter::SetStackSize(int new_stack_size)
{
	if( new_stack_size > stack_size ) {
		::operator delete(stack);
		stack_size = new_stack_size;
		stack = ::operator new(stack_size);/*new CState[stack_size]*/;
	}
}

void CExecuter::SetTableSize(int new_table_size)
{
	if( new_table_size > table_size ) {
		delete[] table;
		table_size = new_table_size;
		table = new TUnitNode*[table_size];
	}
}

const char* names[] = {
	"OT_goto", /* one argument operation pointer */
	"OT_insert_jump", /* one argument operation pointer */
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
	"OT_insert_left_paren", /* no arguments */
	"OT_insert_right_paren", /* no arguments */
	"OT_insert_right_bracket", /* no arguments */
	"OT_move_s", /* one argument offset in table */
	"OT_copy_s", /* one argument offset in table */
	"OT_move_e", /* one argument offset in table */
	"OT_move_wv", /* one argument offset in table */
	"OT_copy_wve", /* one argument offset in table */
	"OT_return", /* one argument points array size */
};

void print(COperation* operation)
{
	std::cout << names[operation->Type()] << "\n";
}

void CExecuter::Run(COperation* operation, TUnitNode* first, TUnitNode* last)
{
	lb = first;
	rb = last;
	op = operation;
	table_index = 0;
	stack_depth = 0;

	while( true ) {
		// std::cout << stack_depth << " "; print(op);
		switch( op->Type() ) {
			case COperation::OT_goto:
				op = op->Operation()->operation;
				break;
					
			case COperation::OT_insert_jump:
				save();
				states_stack(stack_depth).op = op->Operation()->operation;
				++stack_depth;
				COperationOperation::Next(op);
				break;

			case COperation::OT_matching_complete:
				first = first->Prev();
				lb = 0;
				rb = &master_term;
				stack_depth = 0;
				master_term.PairedParen() = 0;
				COperation::Next(op);
				break;

			/* left part operations */
			case COperation::OT_empty_expression_match:
				if( lb->Next() == rb ) {
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_left_symbol_match:
					
				if( shift_left() && CompareUnit( *lb, op->Unit()->value ) ) {
					table[table_index] = lb;
					++table_index;
					COperationUnit::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_symbol_match:
				if( shift_right() && CompareUnit( *rb, op->Unit()->value ) ) {
					table[table_index] = rb;
					++table_index;
					COperationUnit::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_left_parens_match:
				if( shift_left() && lb->IsLeftParen() ) {
					rb = lb->PairedParen();
					table[table_index] = lb;
					++table_index;
					table[table_index] = rb;
					++table_index;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_parens_match:
				if( shift_right() && rb->IsRightParen() ) {
					lb = rb->PairedParen();
					table[table_index] = lb;
					++table_index;
					table[table_index] = rb;
					++table_index;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_set_pointers:
				lb = table[op->IntInt()->x];
				rb = table[op->IntInt()->y];
				COperationIntInt::Next(op);
				break;

			case COperation::OT_left_s_variable_match:
				if( shift_left() && lb->IsSymbol() ) {
					table[table_index] = lb;
					++table_index;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_s_variable_match:
				if( shift_right() && rb->IsSymbol() ) {
					table[table_index] = rb;
					++table_index;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_left_duplicate_s_variable_match:
				if( shift_left() && CompareUnit( *lb, *table[op->Int()->x] ) )
				{
					table[table_index] = lb;
					++table_index;
					COperationInt::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_duplicate_s_variable_match:
				if( shift_right() && CompareUnit( *rb, *table[op->Int()->x] ) )
				{
					table[table_index] = rb;
					++table_index;
					COperationInt::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_closed_e_variable_match:
				table[table_index] = lb->Next();
				++table_index;
				table[table_index] = rb->Prev();
				++table_index;
				COperation::Next(op);
				break;

			case COperation::OT_left_duplicate_wve_variable_match:
				// TODO: check
				COperationInt::Next(op);
				break;

			case COperation::OT_right_duplicate_wve_variable_match:
				// TODO: check
				COperationInt::Next(op);
				break;

			case COperation::OT_left_w_variable_match:
				if( shift_left() ) {
					table[table_index] = lb;
					++table_index;
					if( lb->IsLeftParen() )	{
						lb = lb->PairedParen();
					}
					table[table_index] = lb;
					++table_index;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_w_variable_match:
				if( shift_right() ) {
					table[table_index + 1] = rb;
					if( rb->IsRightParen() ) 	{
						rb = rb->PairedParen();
					}
					table[table_index] = rb;
					table_index += 2;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_check_not_empty:
				if( table[table_index - 1]->Next() ==
					table[table_index - 2] ) {
					fail();
				} else {
					COperation::Next(op);
				}
				break;

			case COperation::OT_left_e_variable_match_begin:
				save();
				++stack_depth;
				table[table_index] = lb->Next();
				++table_index;
				table[table_index] = lb;
				++table_index;
				COperation::Next(op);
				if( op->Is(COperation::OT_left_ve_variable_match) ) {
					COperation::Next(op);
				} else {
					COperationQualifier::Next(op);
				}
				break;

			case COperation::OT_left_v_variable_match_begin:
				save();
				table[table_index] = lb->Next();
				table[table_index + 1] = lb;
				COperation::Next(op);
				break;

			case COperation::OT_left_ve_variable_match:
				lb = table[table_index + 1];
				if( shift_left() ) {
					if( lb->IsLeftParen() ) {
						lb = lb->PairedParen();
					}
					++stack_depth;
					table[table_index + 1] = lb;
					table_index += 2;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_left_ve_variable_with_qualifier_match:
				lb = table[table_index + 1];
				if( shift_left() ) {
					if( lb->IsLeftParen() ) {
						lb = lb->PairedParen();
					}
					/* check qualifier */
					if( op->Qualifier()->qualifier->Check(lb) ) {
						++stack_depth;
						table[table_index + 1] = lb;
						table_index += 2;
						COperationQualifier::Next(op);
					} else {
						fail();
					}
				} else {
					fail();
				}
				break;

			case COperation::OT_right_e_variable_match_begin:
				save();
				++stack_depth;
				table[table_index] = rb;
				++table_index;
				table[table_index] = rb->Prev();
				++table_index;
				COperation::Next(op);
				if( op->Is(COperation::OT_right_ve_variable_match) ) {
					COperation::Next(op);
				} else {
					COperationQualifier::Next(op);
				}
				break;

			case COperation::OT_right_v_variable_match_begin:
				save();
				table[table_index] = rb;
				++table_index;
				table[table_index] = rb->Prev();
				++table_index;
				COperation::Next(op);
				break;

			case COperation::OT_right_ve_variable_match:
				rb = table[table_index];
				if( shift_right() ) {
					if( rb->IsRightParen() ) {
						rb = rb->PairedParen();
					}
					++stack_depth;
					table[table_index] = rb;
					table_index += 2;
					COperation::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_right_ve_variable_with_qualifier_match:
				rb = table[table_index];
				if( shift_right() ) {
					if( rb->IsRightParen() ) {
						rb = rb->PairedParen();
					}
					/* check qualifier */
					if( op->Qualifier()->qualifier->Check(rb) ) {
						++stack_depth;
						table[table_index] = rb;
						table_index += 2;
						COperationQualifier::Next(op);
					} else {
						fail();
					}
				} else {
					fail();
				}
				break;

			case COperation::OT_decriase_stack_depth:
				stack_depth -= operation->Int()->x;
				COperationInt::Next(op);
				break;

			case COperation::OT_check_qualifier_last_s_variable:
				if( op->Qualifier()->qualifier->
					Check(table[table_index - 1]) )
				{
					COperationQualifier::Next(op);
				} else {
					fail();
				}
				break;

			case COperation::OT_check_qualifier_last_wve_variable:
				{
					TUnitNode* i = table[table_index - 2]->Prev();
					TUnitNode* till = table[table_index - 1];
					while( i != till ) {
						i = i->Next();
						if( i->IsLeftParen() ) {
							i = i->PairedParen();
						}
						if( !( op->Qualifier()->qualifier->Check(i) ) ) {
							fail();
							break;
						}
					}
					if( i == till ) {
						COperationQualifier::Next(op);
					}
				}
				break;

			case COperation::OT_left_max_qualifier:
				lb = lb->Next();
				table[table_index] = lb;
				++table_index;
				for( ; lb != rb; lb = lb->Next() ) {
					if( !( op->Qualifier()->qualifier->Check(lb) ) ) {
						break;
					} else if( lb->IsLeftParen() ) {
						lb = lb->PairedParen();
					}
				}
				lb = lb->Prev();
				table[table_index] = lb;
				++table_index;
				COperationQualifier::Next(op);
				break;

			case COperation::OT_right_max_qualifier:
				rb = rb->Prev();
				table[table_index + 1] = lb;
				for( ; rb != lb; rb = rb->Prev() ) {
					if( !( op->Qualifier()->qualifier->Check(rb) ) ) {
						break;
					} else if( rb->IsRightParen() ) {
						rb = rb->PairedParen();
					}
				}
				rb = rb->Next();
				table[table_index] = rb;
				table_index += 2;
				COperationQualifier::Next(op);
				break;

			/* right part operations */
			case COperation::OT_insert_symbol:
				//first = CFieldOfView::Insert(first, op->Unit()->value);
				COperationUnit::Next(op);
				break;

			case COperation::OT_insert_left_paren:
				{
					/*CUnitLink* left_paren = CFieldOfView::Insert(first,
						CUnitValue(CLink::T_left_paren));
					left_paren->PairedParen() = lb;
					lb = left_paren;
					first = left_paren;*/
				}
				COperation::Next(op);
				break;

			case COperation::OT_insert_right_paren:
				{
					/*CUnitLink* right_paren = CFieldOfView::Insert(first,
						CUnitValue(CLink::T_right_paren));
					CUnitLink* left_paren = lb;
					lb = lb->PairedParen();
					right_paren->PairedParen() = left_paren;
					left_paren->PairedParen() = right_paren;
					first = right_paren;*/
				}
				COperation::Next(op);
				break;

			case COperation::OT_insert_right_bracket:
				{
					/*CUnitLink* right_paren = CFieldOfView::Insert(first,
						CUnitValue(CLink::T_right_paren));
					CUnitLink* left_paren = lb;
					lb = lb->PairedParen();
					right_paren->PairedParen() = left_paren;
					rb->PairedParen() = right_paren;
					rb = left_paren;
					first = right_paren;*/
				}
				COperation::Next(op);
				break;
				
			case COperation::OT_copy_s:
				//first = CFieldOfView::Copy(first, table[op->Int()->x]);
				COperationInt::Next(op);
				break;

			case COperation::OT_move_e:
				if( table[op->Int()->x]->Next() ==
					table[op->Int()->x - 1] )
				{
					COperationInt::Next(op);
					break;
				}
			case COperation::OT_move_wv:
			case COperation::OT_move_s:
				moves_stack(stack_depth).location = first;
				moves_stack(stack_depth).op = op;
				++stack_depth;
				COperationInt::Next(op);
				break;

			case COperation::OT_copy_wve:
				/*first = CFieldOfView::Copy(first, table[op->Int()->x - 1],
					table[op->Int()->x]);*/
				COperationInt::Next(op);
				break;

			case COperation::OT_return:
				/* do all moves */
				for( --stack_depth; stack_depth >= 0; --stack_depth ) {
					op = moves_stack(stack_depth).op;
					if( op->Is(COperation::OT_move_s) ) {
						/*CFieldOfView::Move(moves_stack(stack_depth).location,
							table[op->Int()->x]);*/
					} else {
						/*CFieldOfView::Move(moves_stack(stack_depth).location,
							table[op->Int()->x - 1], table[op->Int()->x]);*/
					}
				}

				/* remove master term */
				//CFieldOfView::Remove(last->PairedParen(), last);

#if 0
				/* calling following functions in field of view */
				{
					CUnitLink* tmp = 0;
					for( last = master_term.PairedParen(); last != 0; )	{
						first = last->PairedParen();
						CUnitLink* tmp = first->Next();
						if( tmp->IsLabel() ) {
							TLabel label = tmp->Label();
							//CFieldOfView::Remove(tmp);
							tmp = first->PairedParen();
							//try {
								Run(label->second.operation, first, last);
							//} catch(...) {
								// TODO: throw recognition impossible
							//}
						} else {
							// TODO: throw not a label after <
							std::cout << "not a label after < \n";
						}
						last = tmp;
					}
				}
#endif
				return;
				break;
			default:
				/* assert */
				break;
		}
	}
}
#endif

// ---------------------------------------------------------------------------

COperationsExecuter::COperationsExecuter()
{
}

void COperationsExecuter::doOperation()
{
	COperation* op;

	switch( op->type ) {
		case OT_Goto:
			break;
		case OT_InsertJump:
			break;
		case OT_MatchingComplete:
			break;
		case OT_Return:
			break;
		case OT_MatchEmptyExpression:
			break;
		case OT_MatchLeftChar:
			break;
		case OT_MatchLeftLabel:
			break;
		case OT_MatchLeftNumber:
			break;
		case OT_MatchRightChar:
			break;
		case OT_MatchRightLabel:
			break;
		case OT_MatchRightNumber:
			break;
		case OT_MatchLeftParens:
			break;
		case OT_MatchRightParens:
			break;
		case OT_SetLeftBorder:
			break;
		case OT_SetRightBorder:
			break;
		case OT_MatchLeft_S:
			break;
		case OT_MatchRight_S:
			break;
		case OT_MatchLeftDuplicate_S:
			break;
		case OT_MatchRightDuplicate_S:
			break;
		case OT_MatchLeft_W:
			break;
		case OT_MatchRight_W:
			break;
		case OT_MatchClosed_E:
			break;
		case OT_MatchLeftDuplicate_WVE:
			break;
		case OT_MatchRightDuplicate_WVE:
			break;
		case OT_DecrementStackDepth:
			break;
		case OT_CheckNotEmpty:
			break;
		case OT_CheckQualifier_S:
			break;
		case OT_CheckQualifier_WVE:
			break;
		case OT_MacthLeftMaxByQualifier:
			break;
		case OT_MacthRightMaxByQualifier:
			break;
		case OT_MatchLeftBegin_E:
			break;
		case OT_MatchLeftBegin_V:
			break;
		case OT_MatchLeftWithQulifierBegin_V:
			break;
		case OT_MatchLeft_E:
			break;
		case OT_MatchLeftWithQulifier_E:
			break;
		case OT_MatchRightBegin_E:
			break;
		case OT_MatchRightBegin_V:
			break;
		case OT_MatchRightWithQulifierBegin_V:
			break;
		case OT_MatchRight_E:
			break;
		case OT_MatchRightWithQulifier_E:
			break;
		case OT_InsertChar:
			break;
		case OT_InsertLabel:
			break;
		case OT_InsertNumber:
			break;
		case OT_InsertLeftParen:
			break;
		case OT_InsertRightParen:
			break;
		case OT_InsertRightBracket:
			break;
		case OT_Move_S:
			break;
		case OT_Copy_S:
			break;
		case OT_Move_E:
			break;
		case OT_Copy_E:
			break;
		case OT_Move_WV:
			break;
		case OT_Copy_WV:
			break;
		default:
			assert( false );
	}
}

} // end of namespace refal2