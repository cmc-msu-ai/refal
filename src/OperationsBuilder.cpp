#include <Refal2.h>

#include <iostream>

namespace Refal2 {

void COperationsBuilder::Reset()
{
	operations.Empty();
	savedOperation = 0;
	numberOfOperations = 0;
}

void COperationsBuilder::Export( COperationList& saveTo )
{
	saveTo.Append( &operations );
	Reset();
}

void COperationsBuilder::AddMatchingComplete()
{
	std::cout << "COperationsBuilder::AddMatchingComplete\n";
	addNoArgumensOperation( OT_MatchingComplete );
}

void COperationsBuilder::AddReturn()
{
	std::cout << "COperationsBuilder::AddReturn\n";
	addNoArgumensOperation( OT_Return );
	if( savedOperation != 0 ) {
		operations.InsertBefore( operations.GetFirst(),
			COperation( OT_InsertJump ) )->operation = savedOperation;
	}
	savedOperation = operations.GetLast();
}

void COperationsBuilder::AddDecrementStackDepth( const int count )
{
	std::cout << "COperationsBuilder::AddDecrementStackDepth\n";
	addStackDecrementOperation( count );
}

void COperationsBuilder::AddSetLeftBorder(
	const TTableIndex tableIndex )
{
	std::cout << "COperationsBuilder::AddSetLeftBorder\n";
	addTableIndexOperation( OT_SetLeftBorder, tableIndex );
}

void COperationsBuilder::AddSetRightBorder(
	const TTableIndex tableIndex )
{
	std::cout << "COperationsBuilder::AddSetRightBorder\n";
	addTableIndexOperation( OT_SetRightBorder, tableIndex );
}

/* matching operation */
void COperationsBuilder::AddMatchEmptyExpression()
{
	std::cout << "COperationsBuilder::AddMatchEmptyExpression\n";
	addNoArgumensOperation( OT_MatchEmptyExpression );
}

void COperationsBuilder::AddMatchLeftChar( const TChar c )
{
	std::cout << "COperationsBuilder::AddMatchLeftChar\n";
	addCharOperation( OT_MatchLeftChar, c );
}

void COperationsBuilder::AddMatchLeftLabel( const TLabel label )
{
	std::cout << "COperationsBuilder::AddMatchLeftLabel\n";
	addLabelOperation( OT_MatchLeftLabel, label );
}

void COperationsBuilder::AddMatchLeftNumber( const TNumber number )
{
	std::cout << "COperationsBuilder::AddMatchLeftNumber\n";
	addNumberOperation( OT_MatchLeftNumber, number );
}

void COperationsBuilder::AddMatchRightChar( const TChar c )
{
	std::cout << "COperationsBuilder::AddMatchRightChar\n";
	addCharOperation( OT_MatchRightChar, c );
}

void COperationsBuilder::AddMatchRightLabel( const TLabel label )
{
	std::cout << "COperationsBuilder::AddMatchRightLabel\n";
	addLabelOperation( OT_MatchRightLabel, label );
}

void COperationsBuilder::AddMatchRightNumber( const TNumber number )
{
	std::cout << "COperationsBuilder::AddMatchRightNumber\n";
	addNumberOperation( OT_MatchRightNumber, number );
}

void COperationsBuilder::AddMatchLeftParens()
{
	std::cout << "COperationsBuilder::AddMatchLeftParens\n";
	addNoArgumensOperation( OT_MatchLeftParens );
}

void COperationsBuilder::AddMatchRightParens()
{
	std::cout << "COperationsBuilder::AddMatchRightParens\n";
	addNoArgumensOperation( OT_MatchRightParens );
}

void COperationsBuilder::AddMatchLeft_S( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeft_S\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_S : OT_MatchLeft_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_S :
			OT_MatchLeftWithQualifier_S ), qualifier );
	}
}

void COperationsBuilder::AddMatchRight_S( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRight_S\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_S : OT_MatchRight_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_S :
			 OT_MatchRightWithQualifier_S ), qualifier );
	}
}

void COperationsBuilder::AddMatchLeftDuplicate_S(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_S\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_S : OT_MatchLeftDuplicate_S ),
		tableIndex );
}


void COperationsBuilder::AddMatchRightDuplicate_S(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_S\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_S : OT_MatchRightDuplicate_S ),
		tableIndex );
}


void COperationsBuilder::AddMatchLeft_W( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeft_W\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_W : OT_MatchLeft_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_W :
			OT_MatchLeftWithQualifier_W ), qualifier );
	}
}

void COperationsBuilder::AddMatchRight_W( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRight_W\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_W : OT_MatchRight_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_W :
			 OT_MatchRightWithQualifier_W ), qualifier );
	}
}

void COperationsBuilder::AddMatchLeftDuplicate_WV(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_WV\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_WV : OT_MatchLeftDuplicate_WV ),
		tableIndex );
}

void COperationsBuilder::AddMatchRightDuplicate_WV(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_WV\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_WV : OT_MatchRightDuplicate_WV ),
		tableIndex );
}

void COperationsBuilder::AddMatchLeftDuplicate_E(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeftDuplicate_E\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_E : OT_MatchLeftDuplicate_E ),
		tableIndex );
}


void COperationsBuilder::AddMatchRightDuplicate_E(
	const TTableIndex tableIndex, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRightDuplicate_E\n";
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_E : OT_MatchRightDuplicate_E ),
		tableIndex );
}

void COperationsBuilder::AddMatchClosed_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchClosed_V\n";
	if( qualifier->IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchClosedSaveToTable_V : OT_MatchClosed_V ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchClosedWithQualifierSaveToTable_V :
			 OT_MatchClosedWithQualifier_V ), qualifier );
	}
}

void COperationsBuilder::AddMatchClosed_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchClosed_E\n";
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
void COperationsBuilder::AddMacthLeftMaxByQualifier_V(
	CQualifier* qualifier, const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMacthLeftMaxByQualifier_V\n";
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_V :
		OT_MacthLeftMaxByQualifier_V ), qualifier );

}

void COperationsBuilder::AddMacthRightMaxByQualifier_V(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_V :
		OT_MacthRightMaxByQualifier_V ), qualifier );
}

/* matching E-variables by qualifier */
void COperationsBuilder::AddMacthLeftMaxByQualifier_E(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_E :
		OT_MacthLeftMaxByQualifier_E ), qualifier );
}

void COperationsBuilder::AddMacthRightMaxByQualifier_E(
	CQualifier* qualifier, const bool saveInTable )
{
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_E :
		OT_MacthRightMaxByQualifier_E ), qualifier );
}

/* match left VE-variable */
void COperationsBuilder::AddMatchLeft_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeft_E\n";
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

void COperationsBuilder::AddMatchLeft_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchLeft_V\n";
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
void COperationsBuilder::AddMatchRight_E( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRight_E\n";
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

void COperationsBuilder::AddMatchRight_V( CQualifier* qualifier,
	const bool saveInTable )
{
	std::cout << "COperationsBuilder::AddMatchRight_V\n";
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
void COperationsBuilder::AddInsertChar(const TChar c)
{
	std::cout << "COperationsBuilder::AddInsertChar\n";
	addOperation( OT_InsertChar )->c = c;
}

void COperationsBuilder::AddInsertLabel(const TLabel label)
{
	std::cout << "COperationsBuilder::AddInsertLabel\n";
	addOperation( OT_InsertLabel )->label = label;
}

void COperationsBuilder::AddInsertNumber(const TNumber number)
{
	std::cout << "COperationsBuilder::AddInsertNumber\n";
	addOperation( OT_InsertNumber )->number = number;
}

void COperationsBuilder::AddInsertLeftParen()
{
	std::cout << "COperationsBuilder::AddInsertLeftParen\n";
	addOperation( OT_InsertLeftParen );
}

void COperationsBuilder::AddInsertRightParen()
{
	std::cout << "COperationsBuilder::AddInsertRightParen\n";
	addOperation( OT_InsertRightParen );
}

void COperationsBuilder::AddInsertRightBracket()
{
	std::cout << "COperationsBuilder::AddInsertRightBracket\n";
	addOperation( OT_InsertRightBracket );
}

void COperationsBuilder::AddMove_S(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddMove_S\n";
	addOperation( OT_Move_S )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_S(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_S\n";
	addOperation( OT_Copy_S )->tableIndex = tableIndex;
}

void COperationsBuilder::AddMove_E(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddMove_E\n";
	addOperation( OT_Move_E )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_E(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_E\n";
	addOperation( OT_Copy_E )->tableIndex = tableIndex;
}

void COperationsBuilder::AddMove_WV(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddMove_WV\n";
	addOperation( OT_Move_WV )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_WV(const TTableIndex tableIndex)
{
	std::cout << "COperationsBuilder::AddCopy_WV\n";
	addOperation( OT_Copy_WV )->tableIndex = tableIndex;
}

void COperationsBuilder::addNoArgumensOperation(
	const TOperationType type )
{
	addOperation( type );
}

void COperationsBuilder::addCharOperation( const TOperationType type,
	const TChar c )
{
	addOperation( type )->c = c;
}

void COperationsBuilder::addLabelOperation( const TOperationType type,
	const TLabel label )
{
	addOperation( type )->label = label;
}

void COperationsBuilder::addNumberOperation( const TOperationType type,
	const TNumber number )
{
	addOperation( type )->number = number;
}

void COperationsBuilder::addTableIndexOperation( const TOperationType type,
	const TTableIndex tableIndex )
{
	addOperation( type )->tableIndex = tableIndex;
}

void COperationsBuilder::addQualifierIndexOperation( const TOperationType type,
	CQualifier* qualifier )
{
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void COperationsBuilder::addOperation_VE( const TOperationType type )
{
	addOperation( type );
}

void COperationsBuilder::addOperation_VE( const TOperationType type,
	CQualifier* qualifier )
{
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void COperationsBuilder::addStackDecrementOperation(
	const TUint32 stackDecrement )
{
	addOperation( OT_DecrementStackDepth )->stackDecrement = stackDecrement;
}

TQualifierIndex COperationsBuilder::registerQualifier( CQualifier* qualifier )
{
	CQualifier* newQualifier = new CQualifier;
	qualifier->Move( newQualifier );
	return newQualifier;

}

COperation* COperationsBuilder::addOperation( const TOperationType type )
{
	addOperation( COperation( type ) );
	return operations.GetLast();
}

void COperationsBuilder::addOperation( const COperation& operation )
{
	operations.Append( operation );
	numberOfOperations++;
}

} // end of namespace refal2
