#include "Common.h"

#include <iostream>
#include <string>
#include <map>

namespace Refal2 {

void print_link(const CLink* link)
{  
	switch( link->Type() )
	{
	case CLink::T_char :
		std::cout << "\'" << link->Char() << "\' ";
		break;
	case CLink::T_label :
		std::cout << "/" << link->Label()->first << "/ ";
		break;
	case CLink::T_number :
		std::cout << "/" << link->Number() << "/ ";
		break;
	case CLink::T_variable :
		{
			const CVariableLink* var = link->Variable();
			if( var->qualifier == 0 ) {
				std::cout << var->variable_type << var->variable_name << " ";
			} else {
				std::cout << var->variable_type << "()" <<
					var->variable_name << " ";
			}
		}
		break;
	case CLink::T_left_paren :
		std::cout << "( ";
		break;
	case CLink::T_right_paren :
		std::cout << ") ";
		break;
	case CLink::T_left_bracket :
		std::cout << "< ";
		break;
	case CLink::T_right_bracket :
		std::cout << "> ";
		break;
	default:
		break;
	}
}

void print_rule(const CRule* rule)
{
	if( rule->right_matching ) {
		std::cout << "R ";
	}

	for( CLink* i = rule->left_part_begin; i != 0; i = i->next ) {
		print_link(i);
	}

	std::cout << " = ";
	
	for( CLink* i = rule->right_part_begin; i != 0; i = i->next ) {
		print_link(i);
	}
}


void print_function(const CRule* rule)
{
	for( ; rule != 0; rule = rule->next_rule ) {
		print_rule(rule);
		std::cout << "\n";
	}
}

} // end of namespace Refal2

