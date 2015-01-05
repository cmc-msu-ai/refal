#include "LinkBuilder.h"

#include "Common.h"

namespace Refal2 {

void CLinkBuilder::Free(CUnitLink** link)
{
	CUnitLink* i = *link;
	*link = 0;

	while( i != 0 ) {
		CUnitLink* tmp = i;
		i = i->next;
		delete tmp;
	}
}

void CLinkBuilder::Reset()
{
	if( begin != 0 ) {
		Free(&begin);
	}

	begin = 0;
	end = 0;
}

void CLinkBuilder::Get(CUnitLink** link_begin, CUnitLink** link_end)
{
	if( link_begin != 0 || link_end != 0 ) {
		if( link_begin != 0 ) {
			*link_begin = begin;
		}

		if( link_end != 0 ) {
			*link_end = end;
		}

		begin = 0;
		end = 0;
	}
}
	
CUnitLink* CLinkBuilder::Char(TChar c)
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_char);
	tmp->Char() = c;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::Label(TLabel label)
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_label);
	tmp->Label() = label;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::Number(TNumber number)
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_number);
	tmp->Number() = number;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::Variable(TVariableIndex variable)
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_variable);
	tmp->Variable() = variable;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::LeftParen()
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_left_paren);
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::RightParen()
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_right_paren);
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::LeftBracket()
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_left_bracket);
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::RightBracket()
{
	CUnitLink* tmp = new CUnitLink(CUnitLink::T_right_bracket);
	link(tmp);
	return tmp;
}

void CLinkBuilder::link(CUnitLink* link)
{
	if( begin == 0 ) {
		begin = link;
		end = link;
	} else {
		link->prev = end;
		end->next = link;
		end = link;
	}
}

} // end of namespace Refal2

