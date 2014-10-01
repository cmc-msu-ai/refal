#include "FieldOfView.h"

#include "Common.h"

namespace Refal2 {

#define CREATE_UNITLINK(X) new(Alloc(sizeof(CUnitLink)))CUnitLink(X)

CFieldOfView::CFieldOfView():
	first(CREATE_UNITLINK(CLink::T_left_paren)),
	last(CREATE_UNITLINK(CLink::T_right_paren))
{
	first->next = last;
	last->prev = first;
	first->PairedParen() = last;
	first->PairedParen() = first;
}

void CFieldOfView::Print() const
{
	for( CUnitLink* i = first; i != 0; i = i->Next() ) {
		print_link(i);
	}
}

CUnitLink* CFieldOfView::Insert(CUnitLink* location, const CUnitValue& value)
{
	CUnitLink* tmp = CREATE_UNITLINK(value.type);
	tmp->value = value.value;
	tmp->next = location->next;
	tmp->prev = location;
	location->next->prev = tmp;
	location->next = tmp;
	return tmp;
}

CUnitLink* CFieldOfView::Insert(CUnitLink* location, CUnitLink* what)
{
	if( location != what ) {
		what->next = location->next;
		what->prev = location;
		location->next->prev = what;
		location->next = what;
		return what;
	}
	return location;
}

CUnitLink* CFieldOfView::Insert(CUnitLink* location, CUnitLink* from,
	CUnitLink* to)
{
	if( from->Prev() != to ) {
		to->next = location->next;
		from->prev = location;
		location->next->prev = to;
		location->next = from;
		return to;
	}
	return location;
}

void CFieldOfView::Detach(CUnitLink* location)
{
	if( location->prev != 0 ) {
		location->prev->next = location->next;
	}
	if( location->next != 0 ) {
		location->next->prev = location->prev;
	}
	location->prev = 0;
	location->next = 0;
}

void CFieldOfView::Detach(CUnitLink* from, CUnitLink* to)
{
	if( from->Prev() != to ) {

		if( from->prev != 0 ) {
			from->prev->next = to->next;
		}
		if( to->next != 0 ) {
			to->next->prev = from->prev;
		}
		from->prev = 0;
		to->next = 0;
	}
}

void CFieldOfView::Remove(CUnitLink* location)
{
	Detach(location);
	Free(location);
}

void CFieldOfView::Remove(CUnitLink* from, CUnitLink* to)
{
	if( from->Prev() != to ) {
		Detach(from, to);
		while( true ) {
			if( from == to ) {
				Free(from);
				break;
			} else {
				CUnitLink* tmp = from;
				from = from->Next();
				Free(tmp);
			}
		}
	}
}

CUnitLink* CFieldOfView::Move(CUnitLink* location, CUnitLink* what)
{
	if( location != what ) {
		Detach(what);
		return Insert(location, what);
	}
	return location;
}

CUnitLink* CFieldOfView::Move(CUnitLink* location, CUnitLink* from, CUnitLink* to)
{
	if( from->Prev() != to ) {
		Detach(from, to);
		return Insert(location, from, to);
	}
	return location;
}

CUnitLink* CFieldOfView::Copy(CUnitLink* location, CUnitLink* what)
{
	CUnitLink* tmp = CREATE_UNITLINK(*what);
	return Insert(location, tmp);
}

CUnitLink* CFieldOfView::Copy(CUnitLink* location, CUnitLink* from, CUnitLink* to)
{
	CFieldOfView(from, to).Print();
	
	if( from->Prev() != to ) {
		CUnitLink* begin = CREATE_UNITLINK(*from);
		CUnitLink* end = begin;
	
		while( from != to ) {
			from = from->Next();
			CUnitLink* tmp = CREATE_UNITLINK(*from);
			tmp->prev = end;
			end->next = tmp;
			end = tmp;
		}

		return Insert(location, begin, end);
	}
	return location;
}

void* CFieldOfView::Alloc(int size)
{
	return ::operator new(size);
}

void CFieldOfView::Free(void* what)
{
	::operator delete(what);
}

} // end of namespace refal2
