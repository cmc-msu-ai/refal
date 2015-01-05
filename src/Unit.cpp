#include <stdio.h>
#include <Refal2.h>

namespace Refal2 {

void PrintUnit(const CUnit& unit)
{
	switch( unit.GetType() ) {
		case UT_Char:
			printf("'%c' ", unit.Char());
			break;
		case UT_Label:
			printf("/L%d/ ", unit.Label());
			break;
		case UT_Number:
			printf("/N%d/ ", unit.Number());
			break;
		case UT_Variable:
			printf("V%d ", unit.Variable());
			break;
		case UT_LeftParen:
			printf("( ");
			break;
		case UT_RightParen:
			printf(") ");
			break;
		case UT_LeftBracket:
			printf("< ");
			break;
		case UT_RightBracket:
			printf("> ");
			break;
		default:
			/* assert */
			break;
	}
}

void PrintUnitList(const CUnitNode* fromNode, const CUnitNode* toNode)
{
	if( fromNode == 0 || toNode == 0 ) {
		return;
	}
	
	for( ; fromNode != toNode; fromNode = fromNode->Next() ) {
		PrintUnit(*fromNode);
	}
	PrintUnit(*fromNode);
}

bool CompareUnit(const CUnit& unitA, const CUnit& unitB)
{
	if( unitA.type != unitB.type ) {
		return false;
	}

	switch( unitA.type ) {
		case UT_Char:
			return ( unitA.c == unitB.c );
			break;
		case UT_Label:
			return ( unitA.label == unitB.label );
			break;
		case UT_Number:
			return ( unitA.number == unitB.number );
			break;
		case UT_Variable:
			return ( unitA.variable == unitB.variable );
			break;
		case UT_LeftParen:
		case UT_RightParen:
		case UT_LeftBracket:
		case UT_RightBracket:
			return true;
			break;
		default:
			break;
	}
	assert( false );
	return false;
}

void CUnitList::Assign(CUnitNode* _first, CUnitNode* _last)
{
	Empty();
	first = _first;
	last = _last;
}

void CUnitList::Empty()
{
	if( first != 0 ) {
		Remove(first, last);
	}
	first = 0;
	last = 0;
}

void CUnitList::Swap(CUnitList* listToSwap)
{
	CUnitNode* tmp = first;
	first = listToSwap->first;
	listToSwap->first = tmp;
	
	tmp = last;
	last = listToSwap->last;
	listToSwap->last = tmp;
}

void CUnitList::Move(CUnitList* listTo)
{
	listTo->Empty();
	listTo->first = first;
	listTo->last = last;
	first = 0;
	last = 0;
}
	
CUnitNode* CUnitList::InsertBefore(CUnitNode* nodeBefore, const CUnit& unit)
{
	CUnitNode* newNode = alloc(unit);
	
	newNode->next = nodeBefore;
	newNode->prev = nodeBefore->prev;
	if( nodeBefore == first ) {
		first = newNode;
	} else {
		nodeBefore->prev->next = newNode;
	}
	nodeBefore->prev = newNode;
	
	return newNode;
}

void CUnitList::InsertAfter(CUnitNode* nodeAfter, CUnitNode* fromNode,
	CUnitNode* toNode)
{
	Detach(fromNode, toNode);

	toNode->next = nodeAfter->next;
	fromNode->prev = nodeAfter;
	if( nodeAfter == last ) {
		last = toNode;
	} else {
		nodeAfter->next->prev = toNode;
	}
	nodeAfter->next = fromNode;
}

void CUnitList::Detach(CUnitNode* fromNode, CUnitNode* toNode)
{
	if( fromNode->prev != 0 ) {
		fromNode->prev->next = toNode->next;
	}
	if( toNode->next != 0 ) {
		toNode->next->prev = fromNode->prev;
	}
	if( fromNode == first ) {
		first = toNode->next;
	}
	if( toNode == last ) {
		last = fromNode->prev;
	}
	fromNode->prev = 0;
	toNode->next = 0;
}

void CUnitList::Remove(CUnitNode* fromNode, CUnitNode* toNode)
{
	Detach(fromNode, toNode);
	
	while( fromNode != toNode ) {
		CUnitNode* tmp = fromNode;
		fromNode = fromNode->next;
		free(tmp);
	}
	free(fromNode);
}

void CUnitList::Copy(CUnitNode* nodeAfter, CUnitNode* fromNode,
	CUnitNode* toNode)
{
	CUnitNode* fromNodeCopy = 0;
	CUnitNode* toNodeCopy = 0;
	Duplicate(fromNode, toNode, &fromNodeCopy, &toNodeCopy);
	InsertAfter(nodeAfter, fromNodeCopy, toNodeCopy);
}

CUnitNode* CUnitList::Append(const CUnit& unit)
{
	if( last != 0 ) {
		InsertAfter(last, unit);
	} else {
		first = alloc(unit);
		last = first;
	}
	return last;
}

void CUnitList::Duplicate(const CUnitNode* fromNode, const CUnitNode* toNode,
	CUnitNode** fromNodeCopy, CUnitNode** toNodeCopy)
{
	CUnitNode* tmpFirst = alloc(*fromNode);
	CUnitNode* tmpLast = tmpFirst;
	
	while( fromNode != toNode ) {
		fromNode = fromNode->next;
		
		tmpLast->next = alloc(*fromNode);
		tmpLast->next->prev = tmpLast;
		tmpLast = tmpLast->next;
	}
	
	*fromNodeCopy = tmpFirst;
	*toNodeCopy = tmpLast;
}

CUnitNode* CUnitList::alloc(TUnitType type)
{
	return new CUnitNode(type);
}

CUnitNode* CUnitList::alloc(const CUnit& unit)
{
	return new CUnitNode(unit);
}

void CUnitList::free(CUnitNode* node)
{
	delete node;
}

} // end of namespace Refal2
