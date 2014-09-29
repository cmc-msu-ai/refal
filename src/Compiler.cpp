#include "Compiler.h"

#include "OperationsBuilder.h"
#include "Common.h"
#include <stack>

namespace Refal2 {

CCompiler::CCompiler()
{
	Reset();
}

CCompiler::~CCompiler()
{
}

void CCompiler::Reset()
{
	current_holes.clear();
	tuples_stack.clear();
}

void CCompiler::Compile(CRule* function)
{
	Reset();

	//current_holes.push_back(CHole(begin, end));
}

void CCompiler::compile_left_part(CLink* first, CLink* last)
{
}

void CCompiler::compile_right_part(CLink* first)
{
#if 0
	COperationsBuilder exe;
	std::stack<COperationInt*> stack;
	COperationInt* allocate_points_array_operation = 0;
	int counter = 0;

	for( ; first != 0; first = first->next ) {
		switch( first->type ) {
			case CLink::T_char:
				/* new symbol char */
				break;
			case CLink::T_label:
				/* new symbol label */
				break;
			case CLink::T_number:
				/* new symbol number */
				break;
			case CLink::T_variable:
				if( first->Variable()->type == 's' ) {
					/* move or copy */
				} else {
					/* move or copy */
				}
				break;
			case CLink::T_left_paren:
				/* new left paren */
				exe.Add(COperation::T_insert_parens);
				break;
			case CLink::T_right_paren:
				/* new right paren */
				exe.Add(COperation::T_jump_right_paren);
				break;
			case CLink::T_left_bracket:
				/* save p */
				if( allocate_points_array_operation == 0 ) {
					allocate_points_array_operation =
						exe.Add(COperation::T_allocate_points_array, 0);
				}
				stack.push(exe.Add(COperation::T_save_point, 0));
				break;
			case CLink::T_right_bracket:
				/* save p */
				exe.Add(COperation::T_save_point, counter + 1);
				stack.top()->x = counter;
				stack.pop();
				counter += 2;
				break;
			default:
				/* assert */
				break;
		}
	}
#endif
}

void CCompiler::process_holes()
{
	current_hole = current_holes.begin();
}

} // end of namespace refal2
