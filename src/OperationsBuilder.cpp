#include <Refal2.h>

#include <iostream>

namespace Refal2 {

void CFirstRuleOperationsBuilder::AddMatchingComplete()
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchingComplete\n";
}

void CFirstRuleOperationsBuilder::AddReturn()
{
	std::cout << "CFirstRuleOperationsBuilder::AddReturn\n";
}

void CFirstRuleOperationsBuilder::AddDecrementStackDepth( const int count )
{
	std::cout << "CFirstRuleOperationsBuilder::AddDecrementStackDepth\n";
	addStackDecrementOperation( count );
}

void CFirstRuleOperationsBuilder::AddSetLeftBorder(
	const TTableIndex tableIndex )
{
	std::cout << "CFirstRuleOperationsBuilder::AddSetLeftBorder\n";
	addTableIndexOperation( OT_SetLeftBorder, tableIndex );
}

void CFirstRuleOperationsBuilder::AddSetRightBorder(
	const TTableIndex tableIndex )
{
	std::cout << "CFirstRuleOperationsBuilder::AddSetRightBorder\n";
	addTableIndexOperation( OT_SetRightBorder, tableIndex );
}

/* matching operation */
void CFirstRuleOperationsBuilder::AddMatchEmptyExpression()
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchEmptyExpression\n";
	addNoArgumensOperation( OT_MatchEmptyExpression );
}

void CFirstRuleOperationsBuilder::AddMatchLeftChar( const TChar c )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftChar\n";
	addCharOperation( OT_MatchLeftChar, c );
}

void CFirstRuleOperationsBuilder::AddMatchLeftLabel( const TLabel label )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftLabel\n";
	addLabelOperation( OT_MatchLeftLabel, label );
}

void CFirstRuleOperationsBuilder::AddMatchLeftNumber( const TNumber number )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftNumber\n";
	addNumberOperation( OT_MatchLeftNumber, number );
}

void CFirstRuleOperationsBuilder::AddMatchRightChar( const TChar c )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightChar\n";
	addCharOperation( OT_MatchRightChar, c );
}

void CFirstRuleOperationsBuilder::AddMatchRightLabel( const TLabel label )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightLabel\n";
	addLabelOperation( OT_MatchRightLabel, label );
}

void CFirstRuleOperationsBuilder::AddMatchRightNumber( const TNumber number )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightNumber\n";
	addNumberOperation( OT_MatchRightNumber, number );
}

void CFirstRuleOperationsBuilder::AddMatchLeftParens()
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftParens\n";
	addNoArgumensOperation( OT_MatchLeftParens );
}

void CFirstRuleOperationsBuilder::AddMatchRightParens()
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightParens\n";
	addNoArgumensOperation( OT_MatchRightParens );
}

void CFirstRuleOperationsBuilder::AddMatchLeft_S( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeft_S\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_S : OT_MatchLeft_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_S :
			OT_MatchLeftWithQualifier_S ), qualifier );
	}
}

void CFirstRuleOperationsBuilder::AddMatchRight_S( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRight_S\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_S : OT_MatchRight_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_S :
			 OT_MatchRightWithQualifier_S ), qualifier );
	}
}

void CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_S(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_S\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_S : OT_MatchLeftDuplicate_S ),
		tableIndex );
}


void CFirstRuleOperationsBuilder::AddMatchRightDuplicate_S(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightDuplicate_S\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_S : OT_MatchRightDuplicate_S ),
		tableIndex );
}


void CFirstRuleOperationsBuilder::AddMatchLeft_W( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeft_W\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_W : OT_MatchLeft_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_W :
			OT_MatchLeftWithQualifier_W ), qualifier );
	}
}

void CFirstRuleOperationsBuilder::AddMatchRight_W( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRight_W\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_W : OT_MatchRight_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_W :
			 OT_MatchRightWithQualifier_W ), qualifier );
	}
}

void CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_WV(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_WV\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_WV : OT_MatchLeftDuplicate_WV ),
		tableIndex );
}

void CFirstRuleOperationsBuilder::AddMatchRightDuplicate_WV(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightDuplicate_WV\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_WV : OT_MatchRightDuplicate_WV ),
		tableIndex );
}

void CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_E(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeftDuplicate_E\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_E : OT_MatchLeftDuplicate_E ),
		tableIndex );
}


void CFirstRuleOperationsBuilder::AddMatchRightDuplicate_E(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRightDuplicate_E\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_E : OT_MatchRightDuplicate_E ),
		tableIndex );
}

void CFirstRuleOperationsBuilder::AddMatchClosed_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchClosed_V\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchClosedSaveToTable_V : OT_MatchClosed_V ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchClosedWithQualifierSaveToTable_V :
			 OT_MatchClosedWithQualifier_V ), qualifier );
	}
}

void CFirstRuleOperationsBuilder::AddMatchClosed_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchClosed_E\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchClosedSaveToTable_E : OT_MatchClosed_E ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchClosedWithQualifierSaveToTable_E :
			 OT_MatchClosedWithQualifier_E ), qualifier );
	}
}

/* matching V-variables by qualifier */
void CFirstRuleOperationsBuilder::AddMacthLeftMaxByQualifier_V(
	CQualifier* qualifier, const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMacthLeftMaxByQualifier_V\n";
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_V :
		OT_MacthLeftMaxByQualifier_V ), qualifier );

}

void CFirstRuleOperationsBuilder::AddMacthRightMaxByQualifier_V(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_V :
		OT_MacthRightMaxByQualifier_V ), qualifier );
}

/* matching E-variables by qualifier */
void CFirstRuleOperationsBuilder::AddMacthLeftMaxByQualifier_E(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_E :
		OT_MacthLeftMaxByQualifier_E ), qualifier );
}

void CFirstRuleOperationsBuilder::AddMacthRightMaxByQualifier_E(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_E :
		OT_MacthRightMaxByQualifier_E ), qualifier );
}

/* match left VE-variable */
void CFirstRuleOperationsBuilder::AddMatchLeft_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeft_E\n";
	if( qualifier->IsEmpty() ) {
		if( saveInTable ) {
			addOperation_VE( OT_MatchLeftBeginSaveToTable_E );
			addNoArgumensOperation( OT_MatchLeftBeginSaveToTable_V );
			addNoArgumensOperation( OT_MatchLeftSaveToTable_E );
		} else {
			addOperation_VE( OT_MatchLeftBegin_E );
			addNoArgumensOperation( OT_MatchLeftBegin_V );
			addNoArgumensOperation( OT_MatchLeft_E );
		}
	} else {
		if( saveInTable ) {
			addOperation_VE( OT_MatchLeftBeginSaveToTable_E );
			addQualifierIndexOperation(
				OT_MatchLeftWithQulifierBeginSaveToTable_V, qualifier );
			addQualifierIndexOperation( OT_MatchLeftWithQulifierSaveToTable_E,
				qualifier );
		} else {
			addOperation_VE( OT_MatchLeftBegin_E );
			addQualifierIndexOperation(
				OT_MatchLeftWithQulifierBegin_V, qualifier );
			addQualifierIndexOperation( OT_MatchLeftWithQulifier_E, qualifier );
		}
	}
}

void CFirstRuleOperationsBuilder::AddMatchLeft_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchLeft_V\n";
	if( qualifier->IsEmpty() ) {
		if( saveInTable ) {
			addOperation_VE( OT_MatchLeftBeginSaveToTable_V );
			addNoArgumensOperation( OT_MatchLeftSaveToTable_E );
		} else {
			addOperation_VE( OT_MatchLeftBegin_V );
			addNoArgumensOperation( OT_MatchLeft_E );
		}
	} else {
		if( saveInTable ) {
			addOperation_VE( OT_MatchLeftWithQulifierBeginSaveToTable_V,
				qualifier );
			addQualifierIndexOperation( OT_MatchLeftWithQulifierSaveToTable_E,
				qualifier );
		} else {
			addOperation_VE( OT_MatchLeftWithQulifierBegin_V, qualifier );
			addQualifierIndexOperation( OT_MatchLeftWithQulifier_E, qualifier );
		}
	}
}

/* match right VE-variable */
void CFirstRuleOperationsBuilder::AddMatchRight_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRight_E\n";
	if( qualifier->IsEmpty() ) {
		if( saveInTable ) {
			addOperation_VE( OT_MatchRightBeginSaveToTable_E );
			addNoArgumensOperation( OT_MatchRightBeginSaveToTable_V );
			addNoArgumensOperation( OT_MatchRightSaveToTable_E );
		} else {
			addOperation_VE( OT_MatchRightBegin_E );
			addNoArgumensOperation( OT_MatchRightBegin_V );
			addNoArgumensOperation( OT_MatchRight_E );
		}
	} else {
		if( saveInTable ) {
			addOperation_VE( OT_MatchRightBeginSaveToTable_E );
			addQualifierIndexOperation(
				OT_MatchRightWithQulifierBeginSaveToTable_V, qualifier );
			addQualifierIndexOperation( OT_MatchRightWithQulifierSaveToTable_E,
				qualifier );
		} else {
			addOperation_VE( OT_MatchRightBegin_E );
			addQualifierIndexOperation(
				OT_MatchRightWithQulifierBegin_V, qualifier );
			addQualifierIndexOperation( OT_MatchRightWithQulifier_E,
				qualifier );
		}
	}
}

void CFirstRuleOperationsBuilder::AddMatchRight_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "CFirstRuleOperationsBuilder::AddMatchRight_V\n";
	if( qualifier->IsEmpty() ) {
		if( saveInTable ) {
			addOperation_VE( OT_MatchRightBeginSaveToTable_V );
			addNoArgumensOperation( OT_MatchRightSaveToTable_E );
		} else {
			addOperation_VE( OT_MatchRightBegin_V );
			addNoArgumensOperation( OT_MatchRight_E );
		}
	} else {
		if( saveInTable ) {
			addOperation_VE( OT_MatchRightWithQulifierBeginSaveToTable_V,
				qualifier );
			addQualifierIndexOperation( OT_MatchRightWithQulifierSaveToTable_E,
				qualifier );
		} else {
			addOperation_VE( OT_MatchRightWithQulifierBegin_V, qualifier );
			addQualifierIndexOperation( OT_MatchRightWithQulifier_E,
				qualifier );
		}
	}
}

/* result builder operation */
void CFirstRuleOperationsBuilder::AddInsertChar(const TChar c)
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertChar\n";
	addOperation( OT_InsertChar )->c = c;
}

void CFirstRuleOperationsBuilder::AddInsertLabel(const TLabel label)
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertLabel\n";
	addOperation( OT_InsertLabel )->label = label;
}

void CFirstRuleOperationsBuilder::AddInsertNumber(const TNumber number)
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertNumber\n";
	addOperation( OT_InsertNumber )->number = number;
}

void CFirstRuleOperationsBuilder::AddInsertLeftParen()
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertLeftParen\n";
	addOperation( OT_InsertLeftParen );
}

void CFirstRuleOperationsBuilder::AddInsertRightParen()
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertRightParen\n";
	addOperation( OT_InsertRightParen );
}

void CFirstRuleOperationsBuilder::AddInsertRightBracket()
{
	std::cout << "CFirstRuleOperationsBuilder::AddInsertRightBracket\n";
	addOperation( OT_InsertRightBracket );
}

void CFirstRuleOperationsBuilder::AddMove_S(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddMove_S\n";
	addOperation( OT_Move_S )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::AddCopy_S(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddCopy_S\n";
	addOperation( OT_Copy_S )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::AddMove_E(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddMove_E\n";
	addOperation( OT_Move_E )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::AddCopy_E(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddCopy_E\n";
	addOperation( OT_Copy_E )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::AddMove_WV(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddMove_WV\n";
	addOperation( OT_Move_WV )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::AddCopy_WV(const TTableIndex tableIndex)
{
	std::cout << "CFirstRuleOperationsBuilder::AddCopy_WV\n";
	addOperation( OT_Copy_WV )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::addNoArgumensOperation(
	const TOperationType type )
{
	addOperation( type );
}

void CFirstRuleOperationsBuilder::addCharOperation( const TOperationType type,
	const TChar c )
{
	addOperation( type )->c = c;
}

void CFirstRuleOperationsBuilder::addLabelOperation( const TOperationType type,
	const TLabel label )
{
	addOperation( type )->label = label;
}

void CFirstRuleOperationsBuilder::addNumberOperation( const TOperationType type,
	const TNumber number )
{
	addOperation( type )->number = number;
}

void CFirstRuleOperationsBuilder::addTableIndexOperation(
	const TOperationType type, const TTableIndex tableIndex )
{
	addOperation( type )->tableIndex = tableIndex;
}

void CFirstRuleOperationsBuilder::addQualifierIndexOperation(
	const TOperationType type, CQualifier* qualifier )
{
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void CFirstRuleOperationsBuilder::addOperation_VE( const TOperationType type )
{
	addOperation( type );
}

void CFirstRuleOperationsBuilder::addOperation_VE( const TOperationType type,
	CQualifier* qualifier )
{
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void CFirstRuleOperationsBuilder::addStackDecrementOperation(
	const TUint32 stackDecrement )
{
	addOperation( OT_DecrementStackDepth )->stackDecrement = stackDecrement;
}

TQualifierIndex CFirstRuleOperationsBuilder::registerQualifier(
	CQualifier* qualifier )
{
	return 0;
}

COperation* CFirstRuleOperationsBuilder::addOperation(
	const TOperationType type )
{
	operations.Append( COperation( type ) );
	return operations.GetLast();
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
