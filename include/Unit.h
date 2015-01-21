#pragma once

#include <Refal2.h>

namespace Refal2 {

typedef CNodeList<CUnit>::TNode TUnitNode;

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
bool CompareUnit(const CUnit& unitA, const CUnit& unitB);
void PrintUnitList(const TUnitNode* fromNode, const TUnitNode* toNode,
	const CVariables* variables = 0);
inline void PrintUnitList(const CUnitList& unitList,
	const CVariables* variables);

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
	
	TUnitNode*& PairedParen() { return pairedParen; }
	const TUnitNode* PairedParen() const { return pairedParen; }

private:
	TUnitType type;
	union {
		TChar c;
		TLabel label;
		TNumber number;
		TUnitNode* pairedParen;
		TVariableIndex variable;
	};
};

class CUnitList : public CNodeList<CUnit> {
public:
	CUnitList() {}
	CUnitList(TUnitNode* first, TUnitNode* last): CNodeList( first, last ) {}

	inline TUnitNode* AppendChar(TChar c);
	inline TUnitNode* AppendLabel(TLabel label);
	inline TUnitNode* AppendNumber(TNumber number);
	inline TUnitNode* AppendVariable(TVariableIndex variable);
	inline TUnitNode* AppendLeftParen(TUnitNode* rightParen = 0);
	inline TUnitNode* AppendRightParen(TUnitNode* leftParen = 0);
	inline TUnitNode* AppendLeftBracket(TUnitNode* rightBracket = 0);
	inline TUnitNode* AppendRightBracket(TUnitNode* leftBracket = 0);
};

inline TUnitNode* CUnitList::AppendChar(TChar c)
{
	CUnit unit( UT_Char );
	unit.Char() = c;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendLabel(TLabel label)
{
	CUnit unit( UT_Label );
	unit.Label() = label;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendNumber(TNumber number)
{
	CUnit unit( UT_Number );
	unit.Number() = number;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendVariable(TVariableIndex variable)
{
	CUnit unit( UT_Variable );
	unit.Variable() = variable;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendLeftParen(TUnitNode* rightParen)
{
	CUnit unit( UT_LeftParen );
	unit.PairedParen() = rightParen;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendRightParen(TUnitNode* leftParen)
{
	CUnit unit( UT_RightParen );
	unit.PairedParen() = leftParen;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendLeftBracket(TUnitNode* rightBracket)
{
	CUnit unit( UT_LeftBracket );
	unit.PairedParen() = rightBracket;
	return Append( unit );
}

inline TUnitNode* CUnitList::AppendRightBracket(TUnitNode* leftBracket)
{
	CUnit unit( UT_RightBracket );
	unit.PairedParen() = leftBracket;
	return Append( unit );
}

inline void PrintUnitList(const CUnitList& unitList,
	const CVariables* variables)
{
	PrintUnitList( unitList.GetFirst(), unitList.GetLast(), variables );
}

} // end of namespace Refal2
