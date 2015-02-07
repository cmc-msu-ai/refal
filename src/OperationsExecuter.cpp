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
		table = new CUnitNode*[table_size];
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

void CExecuter::Run(COperation* operation, CUnitNode* first, CUnitNode* last)
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
					CUnitNode* i = table[table_index - 2]->Prev();
					CUnitNode* till = table[table_index - 1];
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

COperationsExecuter::COperationsExecuter():
	left( 0 ),
	right( 0 ),
	tableTop( 0 ),
	operation( 0 ),
	stackTop( 0 ),
	lastAddedLeftParen( 0 ),
	lastAddedLeftBracket( 0 ),
	initialLeftBracket( CUnit( UT_LeftBracket ) )
{
}

static const char* operationNames[] = {
	"OT_Goto",
	"OT_InsertJump",
	"OT_MatchingComplete",
	"OT_Return",
	"OT_SetLeftBorder",
	"OT_SetRightBorder",
	"OT_DecrementStackDepth",
	"OT_MatchEmptyExpression",
	"OT_MatchLeftChar",
	"OT_MatchLeftLabel",
	"OT_MatchLeftNumber",
	"OT_MatchRightChar",
	"OT_MatchRightLabel",
	"OT_MatchRightNumber",
	"OT_MatchLeftParens",
	"OT_MatchRightParens",
	"OT_MatchLeft_S",
	"OT_MatchLeftSaveToTable_S",
	"OT_MatchLeftWithQualifier_S",
	"OT_MatchLeftWithQualifierSaveToTable_S",
	"OT_MatchRight_S",
	"OT_MatchRightSaveToTable_S",
	"OT_MatchRightWithQualifier_S",
	"OT_MatchRightWithQualifierSaveToTable_S",
	"OT_MatchLeftDuplicate_S",
	"OT_MatchLeftDuplicateSaveToTable_S",
	"OT_MatchRightDuplicate_S",
	"OT_MatchRightDuplicateSaveToTable_S",
	"OT_MatchLeft_W",
	"OT_MatchLeftSaveToTable_W",
	"OT_MatchLeftWithQualifier_W",
	"OT_MatchLeftWithQualifierSaveToTable_W",
	"OT_MatchRight_W",
	"OT_MatchRightSaveToTable_W",
	"OT_MatchRightWithQualifier_W",
	"OT_MatchRightWithQualifierSaveToTable_W",
	"OT_MatchLeftDuplicate_WV",
	"OT_MatchLeftDuplicateSaveToTable_WV",
	"OT_MatchRightDuplicate_WV",
	"OT_MatchRightDuplicateSaveToTable_WV",
	"OT_MatchLeftDuplicate_E",
	"OT_MatchLeftDuplicateSaveToTable_E",
	"OT_MatchRightDuplicate_E",
	"OT_MatchRightDuplicateSaveToTable_E",
	"OT_MatchClosed_V",
	"OT_MatchClosedSaveToTable_V",
	"OT_MatchClosedWithQualifier_V",
	"OT_MatchClosedWithQualifierSaveToTable_V",
	"OT_MatchClosed_E",
	"OT_MatchClosedSaveToTable_E",
	"OT_MatchClosedWithQualifier_E",
	"OT_MatchClosedWithQualifierSaveToTable_E",
	"OT_MacthLeftMaxByQualifier_V",
	"OT_MacthLeftMaxByQualifierSaveToTable_V",
	"OT_MacthRightMaxByQualifier_V",
	"OT_MacthRightMaxByQualifierSaveToTable_V",
	"OT_MacthLeftMaxByQualifier_E",
	"OT_MacthLeftMaxByQualifierSaveToTable_E",
	"OT_MacthRightMaxByQualifier_E",
	"OT_MacthRightMaxByQualifierSaveToTable_E",
	"OT_MatchLeftBegin_E",
	"OT_MatchLeftBeginSaveToTable_E",
	"OT_MatchLeftBegin_V",
	"OT_MatchLeftBeginSaveToTable_V",
	"OT_MatchLeftWithQulifierBegin_V",
	"OT_MatchLeftWithQulifierBeginSaveToTable_V",
	"OT_MatchLeft_E",
	"OT_MatchLeftSaveToTable_E",
	"OT_MatchLeftWithQulifier_E",
	"OT_MatchLeftWithQulifierSaveToTable_E",
	"OT_MatchRightBegin_E",
	"OT_MatchRightBeginSaveToTable_E",
	"OT_MatchRightBegin_V",
	"OT_MatchRightBeginSaveToTable_V",
	"OT_MatchRightWithQulifierBegin_V",
	"OT_MatchRightWithQulifierBeginSaveToTable_V",
	"OT_MatchRight_E",
	"OT_MatchRightSaveToTable_E",
	"OT_MatchRightWithQulifier_E",
	"OT_MatchRightWithQulifierSaveToTable_E",
	"OT_InsertChar",
	"OT_InsertLabel",
	"OT_InsertNumber",
	"OT_InsertLeftParen",
	"OT_InsertRightParen",
	"OT_InsertRightBracket",
	"OT_Move_S",
	"OT_Copy_S",
	"OT_Move_E",
	"OT_Copy_E",
	"OT_Move_WV",
	"OT_Copy_WV"
};

void COperationsExecuter::Run( const TLabel entry )
{
	fieldOfView.Empty();
	left = fieldOfView.Append( CUnit( UT_LeftBracket ) );
	left->PairedParen() = 0;
	fieldOfView.Append( CUnit( UT_Label ) )->Label() = entry;
	right = fieldOfView.Append( CUnit( UT_RightBracket ) );
	right->PairedParen() = left;
	initialLeftBracket.PairedParen() = right;

	while( initialLeftBracket.PairedParen() != 0 ) {
		right = initialLeftBracket.PairedParen();
		left = right->PairedParen()->Next();
		operation = static_cast<COperationNode*>(
			LabelTable.GetLabelFunction( left->Label() )->firstOperation );
		tableTop = 0;
		stackTop = 0;
		lastAddedLeftParen = 0;
		lastAddedLeftBracket = &initialLeftBracket;
		initialLeftBracket.PairedParen() = 0;
		saveToTable( left, right );
		while( !doOperation() ) {
			nextOperation();
		}
	}

	std::cout << "\n\n\n-----------------------------------------\n\n";
	PrintUnitList( fieldOfView );
	std::cout << "\n\n\n-----------------------------------------\n\n";
}

bool COperationsExecuter::doOperation()
{
	std::cout << operationNames[operation->type] << "\n";
	switch( operation->type ) {
		case OT_Goto: /* TOperationAddress */
			assert( false );
			break;
		case OT_InsertJump: /* TOperationAddress */
			insertJump( operation->operation );
			break;
		case OT_MatchingComplete:
			matchingComplete();
			break;
		case OT_Return:
			doReturn();
			return true;
			break;
		case OT_SetLeftBorder: /* TTableIndex */
			setLeftBorder( operation->tableIndex );
			break;
		case OT_SetRightBorder: /* TTableIndex */
			setRightBorder( operation->tableIndex );
			break;
		case OT_DecrementStackDepth: /* TUint32 */
			assert( false );
			break;
		/* matching empty expression */
		case OT_MatchEmptyExpression:
			matchEmptyExpression();
			break;
		/* matching symbols */
		case OT_MatchLeftChar: /* TChar */
			matchLeftChar( operation->c );
			break;
		case OT_MatchLeftLabel: /* TLabel */
			matchLeftLabel( operation->label );
			break;
		case OT_MatchLeftNumber: /* TNumber */
			matchLeftNumber( operation->number );
			break;
		case OT_MatchRightChar: /* TChar */
			matchRightChar( operation->c );
			break;
		case OT_MatchRightLabel: /* TLabel */
			matchRightLabel( operation->label );
			break;
		case OT_MatchRightNumber: /* TNumber */
			matchRightNumber( operation->number );
			break;
		/* matching parens */
		case OT_MatchLeftParens:
			matchLeftParens();
			break;
		case OT_MatchRightParens:
			matchRightParens();
			break;
		/* matching S-variables */
		case OT_MatchLeft_S:
			matchLeft_S();
			break;
		case OT_MatchLeftSaveToTable_S:
			matchLeftSaveToTable_S();
			break;
		case OT_MatchLeftWithQualifier_S: /* TQualifierIndex */
			matchLeftWithQualifier_S( operation->qualifier );
			break;
		case OT_MatchLeftWithQualifierSaveToTable_S: /* TQualifierIndex */
			matchLeftWithQualifierSaveToTable_S( operation->qualifier );
			break;
		case OT_MatchRight_S:
			matchRight_S();
			break;
		case OT_MatchRightSaveToTable_S:
			matchRightSaveToTable_S();
			break;
		case OT_MatchRightWithQualifier_S: /* TQualifierIndex */
			matchRightWithQualifier_S( operation->qualifier );
			break;
		case OT_MatchRightWithQualifierSaveToTable_S: /* TQualifierIndex */
			matchRightWithQualifierSaveToTable_S( operation->qualifier );
			break;
		/* matching duplicate of S-variables */
		case OT_MatchLeftDuplicate_S: /* TTableIndex */
			matchLeftDuplicate_S( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_S: /* TTableIndex */
			matchLeftDuplicateSaveToTable_S( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_S: /* TTableIndex */
			matchRightDuplicate_S( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_S: /* TTableIndex */
			matchRightDuplicateSaveToTable_S( operation->tableIndex );
			break;
		/* matching W-variables */
		case OT_MatchLeft_W:
			matchLeft_W();
			break;
		case OT_MatchLeftSaveToTable_W:
			matchLeftSaveToTable_W();
			break;
		case OT_MatchLeftWithQualifier_W: /* TQualifierIndex */
			matchLeftWithQualifier_W( operation->qualifier );
			break;
		case OT_MatchLeftWithQualifierSaveToTable_W: /* TQualifierIndex */
			matchLeftWithQualifierSaveToTable_W( operation->qualifier );
			break;
		case OT_MatchRight_W:
			matchRight_W();
			break;
		case OT_MatchRightSaveToTable_W:
			matchRightSaveToTable_W();
			break;
		case OT_MatchRightWithQualifier_W: /* TQualifierIndex */
			matchRightWithQualifier_W( operation->qualifier );
			break;
		case OT_MatchRightWithQualifierSaveToTable_W: /* TQualifierIndex */
			matchRightWithQualifierSaveToTable_W( operation->qualifier );
			break;
		/* matching duplicate of WV-variables */
		case OT_MatchLeftDuplicate_WV: /* TTableIndex */
			matchLeftDuplicate_WV( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_WV: /* TTableIndex */
			matchLeftDuplicateSaveToTable_WV( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_WV: /* TTableIndex */
			matchRightDuplicate_WV( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_WV: /* TTableIndex */
			matchRightDuplicateSaveToTable_WV( operation->tableIndex );
			break;
		/* matching duplicate of E-variables */
		case OT_MatchLeftDuplicate_E: /* TTableIndex */
			matchLeftDuplicate_E( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_E: /* TTableIndex */
			matchLeftDuplicateSaveToTable_E( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_E: /* TTableIndex */
			matchRightDuplicate_E( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_E: /* TTableIndex */
			matchRightDuplicateSaveToTable_E( operation->tableIndex );
			break;
		/* matching closed V-variables */
		case OT_MatchClosed_V:
			matchClosed_V();
			break;
		case OT_MatchClosedSaveToTable_V:
			matchClosedSaveToTable_V();
			break;
		case OT_MatchClosedWithQualifier_V: /* TQualifierIndex */
			matchClosedWithQualifier_V( operation->qualifier );
			break;
		case OT_MatchClosedWithQualifierSaveToTable_V: /* TQualifierIndex */
			matchClosedWithQualifierSaveToTable_V( operation->qualifier );
			break;
		/* matching closed E-variables */
		case OT_MatchClosed_E:
			matchClosed_E();
			break;
		case OT_MatchClosedSaveToTable_E:
			matchClosedSaveToTable_E();
			break;
		case OT_MatchClosedWithQualifier_E: /* TQualifierIndex */
			matchClosedWithQualifier_E( operation->qualifier );
			break;
		case OT_MatchClosedWithQualifierSaveToTable_E: /* TQualifierIndex */
			matchClosedWithQualifierSaveToTable_E( operation->qualifier );
			break;
		/* matching V-variables by qualifier */
		case OT_MacthLeftMaxByQualifier_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthLeftMaxByQualifierSaveToTable_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifier_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifierSaveToTable_V: /* TQualifierIndex */
			assert( false );
			break;
		/* matching E-variables by qualifier */
		case OT_MacthLeftMaxByQualifier_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthLeftMaxByQualifierSaveToTable_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifier_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifierSaveToTable_E: /* TQualifierIndex */
			assert( false );
			break;
		/* match left VE-variable */
		case OT_MatchLeftBegin_E:
			matchLeftBegin_E();
			break;
		case OT_MatchLeftBeginSaveToTable_E:
			matchLeftBeginSaveToTable_E();
			break;
		case OT_MatchLeftBegin_V:
			matchLeftBegin_V();
			break;
		case OT_MatchLeftBeginSaveToTable_V:
			matchLeftBeginSaveToTable_V();
			break;
		case OT_MatchLeftWithQulifierBegin_V: /* TQualifierIndex */
			matchLeftWithQulifierBegin_V( operation->qualifier );
			break;
		case OT_MatchLeftWithQulifierBeginSaveToTable_V: /* TQualifierIndex */
			matchLeftWithQulifierBeginSaveToTable_V( operation->qualifier );
			break;
		case OT_MatchLeft_E:
			matchLeft_E();
			break;
		case OT_MatchLeftSaveToTable_E:
			matchLeftSaveToTable_E();
			break;
		case OT_MatchLeftWithQulifier_E: /* TQualifierIndex */
			matchLeftWithQulifier_E( operation->qualifier );
			break;
		case OT_MatchLeftWithQulifierSaveToTable_E: /* TQualifierIndex */
			matchLeftWithQulifierSaveToTable_E( operation->qualifier );
			break;
		/* match right VE-variable */
		case OT_MatchRightBegin_E:
			matchRightBegin_E();
			break;
		case OT_MatchRightBeginSaveToTable_E:
			matchRightBeginSaveToTable_E();
			break;
		case OT_MatchRightBegin_V:
			matchRightBegin_V();
			break;
		case OT_MatchRightBeginSaveToTable_V:
			matchRightBeginSaveToTable_V();
			break;
		case OT_MatchRightWithQulifierBegin_V: /* TQualifierIndex */
			matchRightWithQulifierBegin_V( operation->qualifier );
			break;
		case OT_MatchRightWithQulifierBeginSaveToTable_V: /* TQualifierIndex */
			matchRightWithQulifierBeginSaveToTable_V( operation->qualifier );
			break;
		case OT_MatchRight_E:
			matchRight_E();
			break;
		case OT_MatchRightSaveToTable_E:
			matchRightSaveToTable_E();
			break;
		case OT_MatchRightWithQulifier_E: /* TQualifierIndex */
			matchRightWithQulifier_E( operation->qualifier );
			break;
		case OT_MatchRightWithQulifierSaveToTable_E: /* TQualifierIndex */
			matchRightWithQulifierSaveToTable_E( operation->qualifier );
			break;
		/* making operations */
		case OT_InsertChar: /* TChar */
			insertChar( operation->c );
			break;
		case OT_InsertLabel: /* TLabel */
			insertLabel( operation->label );
			break;
		case OT_InsertNumber: /* TNumber */
			insertNumber( operation->number );
			break;
		case OT_InsertLeftParen:
			insertLeftParen();
			break;
		case OT_InsertRightParen:
			insertRightParen();
			break;
		case OT_InsertRightBracket:
			insertRightBracket();
			break;
		case OT_Move_S: /* TTableIndex */
			move_S( operation->tableIndex );
			break;
		case OT_Copy_S: /* TTableIndex */
			copy_S( operation->tableIndex );
			break;
		case OT_Move_E: /* TTableIndex */
			move_E( operation->tableIndex );
			break;
		case OT_Copy_E: /* TTableIndex */
			copy_E( operation->tableIndex );
			break;
		case OT_Move_WV: /* TTableIndex */
			move_WV( operation->tableIndex );
			break;
		case OT_Copy_WV: /* TTableIndex */
			copy_WV( operation->tableIndex );
			break;
		default:
			assert( false );
			break;
	}
	return false;
}

} // end of namespace refal2