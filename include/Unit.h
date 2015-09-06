#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CPrintHelper

class CPrintHelper {
public:
	CPrintHelper() :
		printLabelWithModule( false )
	{
	}

	bool PrintLabelWithModule() const { return printLabelWithModule; }
	void SetPrintLabelWithModule( bool _printLabelWithModule = true )
	{
		printLabelWithModule = _printLabelWithModule;
	}

	virtual void Variable( std::ostream& outputStream,
		const TVariableIndex variable ) const;
	virtual void Label( std::ostream& outputStream,
		const TLabel label ) const;

private:
	bool printLabelWithModule;

	CPrintHelper( const CPrintHelper& );
	CPrintHelper& operator=( const CPrintHelper& );
};

//-----------------------------------------------------------------------------
// CUnit

typedef CNodeList<CUnit>::CNodeType CUnitNode;

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

class CUnit {
public:
	explicit CUnit( TUnitType _type ): type( _type ) {}
	
	TUnitType GetType() const { return type; }
	void SetType( TUnitType newType ) { type = newType; }
	
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

	bool IsEqualWith( const CUnit& unit ) const;

	void Print( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;

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

//-----------------------------------------------------------------------------
// CUnitList

class CUnitList : public CNodeList<CUnit> {
public:
	CUnitList() {}
	CUnitList(CUnitNode* first, CUnitNode* last): CNodeList( first, last ) {}

	CUnitNode* AppendChar(TChar c);
	CUnitNode* AppendLabel(TLabel label);
	CUnitNode* AppendNumber(TNumber number);
	CUnitNode* AppendVariable(TVariableIndex variable);
	CUnitNode* AppendLeftParen(CUnitNode* rightParen = 0);
	CUnitNode* AppendRightParen(CUnitNode* leftParen = 0);
	CUnitNode* AppendLeftBracket(CUnitNode* rightBracket = 0);
	CUnitNode* AppendRightBracket(CUnitNode* leftBracket = 0);
	CUnitNode* AppendParens();
	void AppendText( const std::string& text );

	static void Print( const CUnitNode* begin, const CUnitNode* end,
		std::ostream& outputStream,	const CPrintHelper& printHelper );
	void Print( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;
	void HandyPrint( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;
	void StrangePrint( std::ostream& outputStream,
		const CPrintHelper& printHelper ) const;

	// CNodeList
	virtual void Duplicate( const CUnitNode* fromNode, const CUnitNode* toNode,
		CUnitNode*& fromNodeCopy, CUnitNode*& toNodeCopy );
};

//-----------------------------------------------------------------------------

inline CUnitNode* CUnitList::AppendChar(TChar c)
{
	CUnit unit( UT_Char );
	unit.Char() = c;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendLabel(TLabel label)
{
	CUnit unit( UT_Label );
	unit.Label() = label;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendNumber(TNumber number)
{
	CUnit unit( UT_Number );
	unit.Number() = number;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendVariable(TVariableIndex variable)
{
	CUnit unit( UT_Variable );
	unit.Variable() = variable;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendLeftParen(CUnitNode* rightParen)
{
	CUnit unit( UT_LeftParen );
	unit.PairedParen() = rightParen;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendRightParen(CUnitNode* leftParen)
{
	CUnit unit( UT_RightParen );
	unit.PairedParen() = leftParen;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendLeftBracket(CUnitNode* rightBracket)
{
	CUnit unit( UT_LeftBracket );
	unit.PairedParen() = rightBracket;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendRightBracket(CUnitNode* leftBracket)
{
	CUnit unit( UT_RightBracket );
	unit.PairedParen() = leftBracket;
	return Append( unit );
}

inline CUnitNode* CUnitList::AppendParens()
{
	CUnitNode* leftParen = AppendLeftParen();
	leftParen->PairedParen() = AppendRightParen( leftParen );
	return leftParen;
}

inline void CUnitList::AppendText( const std::string& text )
{
	for( std::string::const_iterator c = text.begin(); c != text.end(); ++c ) {
		AppendChar( *c );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
