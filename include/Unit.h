#pragma once

#include <Refal2.h>

namespace Refal2 {

enum TUnitType {
	UT_Char = 0x01,
	UT_Label = 0x02,
	UT_Number = 0x04,
	UT_Variable = 0x08,
	UT_LeftParen = 0x10,
	UT_RightParen = 0x20,
	UT_LeftBracket = 0x40,
	UT_RightBracket = 0x80,
};

enum TUnitTypeMask {
	UTM_Paren = UT_LeftParen | UT_RightParen,
	UTM_Symbol = UT_Char | UT_Label | UT_Number,
	UTM_Unit = UTM_Paren | UTM_Symbol,
	UTM_Bracket = UT_LeftBracket | UT_RightBracket
};

void PrintUnit(const CUnit& unit, const CVariables* variables = 0);
void PrintUnitList(const CUnitNode* fromNode, const CUnitNode* toNode,
	const CVariables* variables = 0);
inline void PrintUnitList(const CUnitList& unitList,
	const CVariables* variables = 0);

bool CompareUnit(const CUnit& unitA, const CUnit& unitB);

class CUnit {
	friend bool CompareUnit(const CUnit& unitA, const CUnit& unitB);

public:
	explicit CUnit(TUnitType _type): type(_type) {}
	
	TUnitType GetType() const { return type; }
	
	bool IsVariable() const { return ( (type & UT_Variable) != 0 ); }
	TVariableIndex& Variable() { return variable; }
	const TVariableIndex& Variable() const { return variable; }
	
	bool IsChar() const { return ( (type & UT_Char) != 0 ); }
	TChar& Char() { return c; }
	const TChar& Char() const { return c; }
	
	bool IsLabel() const { return ( (type & UT_Label) != 0 ); }
	TLabel& Label() { return label; }
	const TLabel& Label() const { return label; }
	
	bool IsNumber() const { return ( (type & UT_Number) != 0 ); }
	TNumber& Number() { return number; }
	const TNumber& Number() const { return number; }
	
	bool IsSymbol() const { return ( (type & UTM_Symbol) != 0 ); }
	bool IsUnit() const { return ( (type & UTM_Unit) != 0 ); }
	
	bool IsParen() const { return ( (type & UTM_Paren) != 0 ); }
	bool IsLeftParen() const { return ( (type & UT_LeftParen) != 0 ); }
	bool IsRightParen() const { return ( (type & UT_RightParen) != 0 ); }
	
	bool IsBracket() const { return ( (type & UTM_Bracket) != 0 ); }
	bool IsLeftBracket() const { return ( (type & UT_LeftBracket) != 0 ); }
	bool IsRightBracket() const { return ( (type & UT_RightBracket) != 0 ); }
	
	CUnitNode*& PairedParen() { return pairedParen; }
	const CUnitNode* PairedParen() const { return pairedParen; }

private:
	TUnitType type;
	union {
		TChar c;
		TLabel label;
		TNumber number;
		CUnitNode* pairedParen;
		TVariableIndex variable;
	};
};

class CUnitNode : public CUnit {
	friend class CUnitList;

public:
	explicit CUnitNode(TUnitType _type): CUnit(_type), prev(0), next(0) {}
	CUnitNode(const CUnit& unit): CUnit(unit), prev(0), next(0) {}

	CUnitNode* Next() { return next; }
	const CUnitNode* Next() const { return next; }
	
	CUnitNode* Prev() { return prev; }
	const CUnitNode* Prev() const { return prev; }
	
private:
	CUnitNode* prev;
	CUnitNode* next;
};

class CUnitList {
public:
	CUnitList(): first(0), last(0) {}
	CUnitList(CUnitNode* _first, CUnitNode* _last):
		first(_first), last(_last) {}
	
	~CUnitList() { Empty(); }
	
	void Assign(CUnitNode* _first, CUnitNode* _last);
		
	void Empty();
	bool IsEmpty() { return ( first == 0 ); }
	
	inline void Swap(CUnitList* swapWith);
	inline void Move(CUnitList* moveTo);
	
	CUnitNode* GetFirst() { return first; }
	const CUnitNode* GetFirst() const { return first; }
	
	CUnitNode* GetLast() { return last; }
	const CUnitNode* GetLast() const { return last; }
	
	CUnitNode* InsertBefore(CUnitNode* nodeBefore, const CUnit& unit);
	
	CUnitNode* InsertAfter(CUnitNode* nodeAfter, const CUnit& unit)
		{ return InsertAfter(nodeAfter, alloc(unit)); }
	CUnitNode* InsertAfter(CUnitNode* nodeAfter, CUnitNode* node)
		{ InsertAfter(nodeAfter, node, node); return node; }
	void InsertAfter(CUnitNode* nodeAfter, CUnitNode* fromNode,
		CUnitNode* toNode);
	
	void Detach(CUnitNode* node) { Detach(node, node); }
	void Detach(CUnitNode* fromNode, CUnitNode* toNode);
	
	void Remove(CUnitNode* node) { Detach(node); free(node); }
	void Remove(CUnitNode* fromNode, CUnitNode* toNode);
	
	void Move(CUnitNode* nodeAfter, CUnitNode* node)
		{ InsertAfter(nodeAfter, node); }
	void Move(CUnitNode* nodeAfter, CUnitNode* fromNode, CUnitNode* toNode)
		{ InsertAfter(nodeAfter, fromNode, toNode); }

	void Copy(CUnitNode* nodeAfter, CUnitNode* node)
		{ Copy(nodeAfter, node, node); }
	void Copy(CUnitNode* nodeAfter, CUnitNode* fromNode, CUnitNode* toNode);
	
	CUnitNode* Append(const CUnit& unit);
	inline CUnitNode* AppendChar(TChar c);
	inline CUnitNode* AppendLabel(TLabel label);
	inline CUnitNode* AppendNumber(TNumber number);
	inline CUnitNode* AppendVariable(TVariableIndex variable);
	inline CUnitNode* AppendLeftParen(CUnitNode* rightParen = 0);
	inline CUnitNode* AppendRightParen(CUnitNode* leftParen = 0);
	inline CUnitNode* AppendLeftBracket(CUnitNode* rightBracket = 0);
	inline CUnitNode* AppendRightBracket(CUnitNode* leftBracket = 0);
	
	static void Duplicate(const CUnitNode* fromNode, const CUnitNode* toNode,
		CUnitNode** fromNodeCopy, CUnitNode** toNodeCopy);
	
private:
	static CUnitNode* alloc(TUnitType type);
	static CUnitNode* alloc(const CUnit& unit);
	static void free(CUnitNode* node);

	CUnitNode* first;
	CUnitNode* last;
};

inline void CUnitList::Swap(CUnitList* swapWith)
{
	std::swap( first, swapWith->first );
	std::swap( last, swapWith->last );
}

inline void CUnitList::Move(CUnitList* moveTo)
{
	if( this != moveTo ) {
		moveTo->Empty();
		Swap( moveTo );
	}
}

inline CUnitNode* CUnitList::AppendChar(TChar c)
{
	CUnit unit(UT_Char);
	unit.Char() = c;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendLabel(TLabel label)
{
	CUnit unit(UT_Label);
	unit.Label() = label;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendNumber(TNumber number)
{
	CUnit unit(UT_Number);
	unit.Number() = number;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendVariable(TVariableIndex variable)
{
	CUnit unit(UT_Variable);
	unit.Variable() = variable;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendLeftParen(CUnitNode* rightParen)
{
	CUnit unit(UT_LeftParen);
	unit.PairedParen() = rightParen;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendRightParen(CUnitNode* leftParen)
{
	CUnit unit(UT_RightParen);
	unit.PairedParen() = leftParen;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendLeftBracket(CUnitNode* rightBracket)
{
	CUnit unit(UT_LeftBracket);
	unit.PairedParen() = rightBracket;
	return Append(unit);
}

inline CUnitNode* CUnitList::AppendRightBracket(CUnitNode* leftBracket)
{
	CUnit unit(UT_RightBracket);
	unit.PairedParen() = leftBracket;
	return Append(unit);
}

inline void PrintUnitList(const CUnitList& unitList,
	const CVariables* variables)
{
	PrintUnitList(unitList.GetFirst(), unitList.GetLast(), variables);
}

} // end of namespace Refal2
