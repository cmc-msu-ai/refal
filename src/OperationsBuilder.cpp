#include <Refal2.h>

#include <iostream>

namespace Refal2 {

void COperationsBuilder::AddMatchingComplete()
{
	std::cout << "COperationsBuilder::AddMatchingComplete\n";
}


void COperationsBuilder::AddReturn()
{
	std::cout << "COperationsBuilder::AddReturn\n";
}


/* matching operation */
void COperationsBuilder::AddMatchEmptyExpression()
{
	std::cout << "COperationsBuilder::AddMatchEmptyExpression\n";
}


void COperationsBuilder::AddMatchLeftChar(const TChar c)
{
	std::cout << "COperationsBuilder::AddMatchLeftChar\n";
}


void COperationsBuilder::AddMatchLeftLabel(const TLabel label)
{
	std::cout << "COperationsBuilder::AddMatchLeftLabel\n";
}


void COperationsBuilder::AddMatchLeftNumber(const TNumber number)
{
	std::cout << "COperationsBuilder::AddMatchLeftNumber\n";
}


void COperationsBuilder::AddMatchRightChar(const TChar c)
{
	std::cout << "COperationsBuilder::AddMatchRightChar\n";
}


void COperationsBuilder::AddMatchRightLabel(const TLabel label)
{
	std::cout << "COperationsBuilder::AddMatchRightLabel\n";
}


void COperationsBuilder::AddMatchRightNumber(const TNumber number)
{
	std::cout << "COperationsBuilder::AddMatchRightNumber\n";
}


void COperationsBuilder::AddMatchLeftParens()
{
	std::cout << "COperationsBuilder::AddMatchLeftParens\n";
}


void COperationsBuilder::AddMatchRightParens()
{
	std::cout << "COperationsBuilder::AddMatchRightParens\n";
}


void COperationsBuilder::AddSetLeftBorder(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddSetLeftBorder\n";
}


void COperationsBuilder::AddSetRightBorder(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddSetRightBorder\n";
}


void COperationsBuilder::AddMatchLeft_S(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeft_S\n";
}


void COperationsBuilder::AddMatchRight_S(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRight_S\n";
}


void COperationsBuilder::AddMatchLeftDuplicate_S(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_S\n";
}


void COperationsBuilder::AddMatchRightDuplicate_S(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_S\n";
}


void COperationsBuilder::AddMatchLeft_W(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeft_W\n";
}


void COperationsBuilder::AddMatchRight_W(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRight_W\n";
}

void COperationsBuilder::AddMatchClosed_V(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchClosed_V\n";
}

void COperationsBuilder::AddMatchClosed_E(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchClosed_E\n";
}

void COperationsBuilder::AddMatchLeftDuplicate_WV(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_WV\n";
}


void COperationsBuilder::AddMatchRightDuplicate_WV(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_WV\n";
}

void COperationsBuilder::AddMatchLeftDuplicate_E(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_E\n";
}


void COperationsBuilder::AddMatchRightDuplicate_E(const TTableIndex,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_E\n";
}

void COperationsBuilder::AddDecrementStackDepth(const int count)
{
	std::cout << "COperationsBuilder::AddDecrementStackDepth\n";
}


void COperationsBuilder::AddCheckNotEmpty()
{
	std::cout << "COperationsBuilder::AddCheckNotEmpty\n";
}


/* match left VE-variable */
void COperationsBuilder::AddMatchLeft_E(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeft_E\n";
}


void COperationsBuilder::AddMatchLeft_V(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchLeft_V\n";
}


/* match right VE-variable */
void COperationsBuilder::AddMatchRight_E(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRight_E\n";
}


void COperationsBuilder::AddMatchRight_V(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMatchRight_V\n";
}


/* match by qualifier */
void COperationsBuilder::AddMacthLeftMaxByQualifier(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMacthLeftMaxByQualifier\n";
}


void COperationsBuilder::AddMacthRightMaxByQualifier(CQualifier* qualifier,
	const bool saveInTable)
{
	std::cout << "COperationsBuilder::AddMacthRightMaxByQualifier\n";
}


/* result builder operation */
void COperationsBuilder::AddInsertChar(const TChar c)
{
	std::cout << "COperationsBuilder::AddInsertChar\n";
}


void COperationsBuilder::AddInsertLabel(const TLabel label)
{
	std::cout << "COperationsBuilder::AddInsertLabel\n";
}


void COperationsBuilder::AddInsertNumber(const TNumber number)
{
	std::cout << "COperationsBuilder::AddInsertNumber\n";
}


void COperationsBuilder::AddInsertLeftParen()
{
	std::cout << "COperationsBuilder::AddInsertLeftParen\n";
}


void COperationsBuilder::AddInsertRightParen()
{
	std::cout << "COperationsBuilder::AddInsertRightParen\n";
}


void COperationsBuilder::AddInsertRightBracket()
{
	std::cout << "COperationsBuilder::AddInsertRightBracket\n";
}


void COperationsBuilder::AddMove_S(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddMove_S\n";
}


void COperationsBuilder::AddCopy_S(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_S\n";
}


void COperationsBuilder::AddMove_E(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddMove_E\n";
}


void COperationsBuilder::AddCopy_E(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_E\n";
}


void COperationsBuilder::AddMove_WV(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddMove_WV\n";
}


void COperationsBuilder::AddCopy_WV(const TTableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_WV\n";
}

COperation* COperationsBuilder::addOperation(const TOperationType type)
{
	return 0;
}

} // end of namespace refal2
