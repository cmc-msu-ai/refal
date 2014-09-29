#include "LinkBuilder.h"

#include "Common.h"

namespace Refal2 {

void CLinkBuilder::Free(CLink** link)
{
	CLink* i = *link;
	*link = 0;

	while( i != 0 ) {
		CLink* tmp = i;
		i = i->next;

		if( (tmp->type & CLink::T_unit) != 0 ) {
			delete static_cast<CUnitLink*>(tmp);
		} else if( tmp->type == CLink::T_variable ) {
			delete static_cast<CVariableLink*>(tmp);
		} else {
			delete tmp;
		}

		/*
		switch( tmp->type )
		{
			case CLink::T_char :
				delete static_cast<CCharLink*>(tmp);
				break;
			case CLink::T_label :
				delete static_cast<CLabelLink*>(tmp);
				break;
			case CLink::T_number :
				delete static_cast<CNumberLink*>(tmp);
				break;
			case CLink::T_variable :
				delete static_cast<CVariableLink*>(tmp);
				break;
			case CLink::T_left_paren :
				delete static_cast<CLeftParenLink*>(tmp);
				break;
			case CLink::T_right_paren :
				delete static_cast<CRightParenLink*>(tmp);
				break;
			case CLink::T_left_bracket :
				delete static_cast<CLeftBracketLink*>(tmp);
				break;
			case CLink::T_right_bracket :
				delete static_cast<CRightBracketLink*>(tmp);
				break;
			default:
				break;
		}
		*/
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

void CLinkBuilder::Get(CLink** link_begin, CLink** link_end)
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
	CUnitLink* tmp = new CUnitLink(CLink::T_char);
	tmp->Char() = c;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::Label(TLabel label)
{
	CUnitLink* tmp = new CUnitLink(CLink::T_label);
	tmp->Label() = label;
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::Number(TNumber number)
{
	CUnitLink* tmp = new CUnitLink(CLink::T_number);
	tmp->Number() = number;
	link(tmp);
	return tmp;
}

CVariableLink* CLinkBuilder::Variable(TVariableType type, TVariableName name,
	CQualifier* qualifier)
{
	CVariableLink* tmp = new CVariableLink(type, name, qualifier);
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::LeftParen()
{
	CUnitLink* tmp = new CUnitLink(CLink::T_left_paren);
	link(tmp);
	return tmp;
}

CUnitLink* CLinkBuilder::RightParen()
{
	CUnitLink* tmp = new CUnitLink(CLink::T_right_paren);
	link(tmp);
	return tmp;
}

CLink* CLinkBuilder::LeftBracket()
{
	CLink* tmp = new CLink(CLink::T_left_bracket);
	link(tmp);
	return tmp;
}

CLink* CLinkBuilder::RightBracket()
{
	CLink* tmp = new CLink(CLink::T_right_bracket);
	link(tmp);
	return tmp;
}

void CLinkBuilder::link(CLink* link)
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

