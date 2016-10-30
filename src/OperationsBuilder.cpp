#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// COperationsBuilder

void COperationsBuilder::Reset()
{
	operations.Empty();
	savedOperation = nullptr;
	stackDepth = 0;
	maxStackDepth = 0;
	maxRuleStackDepth = 0;
}

void COperationsBuilder::Export( COperationList& saveTo )
{
	saveTo.Append( operations );
	Reset();
}

void COperationsBuilder::AddMatchingComplete()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_MatchingComplete );
}

void COperationsBuilder::AddReturn()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_Return );
	if( savedOperation != nullptr ) {
		maxStackDepth++;
		operations.InsertBefore( operations.GetFirst(),
			COperation( OT_InsertJump ) )->operation = savedOperation->Next();
	}
	if( maxRuleStackDepth > maxStackDepth ) {
		maxStackDepth = maxRuleStackDepth;
	}
	stackDepth = 0;
	maxRuleStackDepth = 0;
	savedOperation = operations.GetLast();
}

void COperationsBuilder::AddDecrementStackDepth( TStackIndex size )
{
	DEBUG_PRINT( __FUNCTION__ )
	addStackDecrementOperation( size );
}

void COperationsBuilder::AddSetLeftBorder( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( OT_SetLeftBorder, tableIndex );
}

void COperationsBuilder::AddSetRightBorder( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( OT_SetRightBorder, tableIndex );
}

void COperationsBuilder::AddSaveLeftRight()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_SaveLeftRight );
}

/* matching operation */
void COperationsBuilder::AddMatchEmptyExpression()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_MatchEmptyExpression );
}

void COperationsBuilder::AddMatchLeftChar( TChar c )
{
	DEBUG_PRINT( __FUNCTION__ )
	addCharOperation( OT_MatchLeftChar, c );
}

void COperationsBuilder::AddMatchLeftLabel( TLabel label )
{
	DEBUG_PRINT( __FUNCTION__ )
	addLabelOperation( OT_MatchLeftLabel, label );
}

void COperationsBuilder::AddMatchLeftNumber( TNumber number )
{
	DEBUG_PRINT( __FUNCTION__ )
	addNumberOperation( OT_MatchLeftNumber, number );
}

void COperationsBuilder::AddMatchRightChar( TChar c )
{
	DEBUG_PRINT( __FUNCTION__ )
	addCharOperation( OT_MatchRightChar, c );
}

void COperationsBuilder::AddMatchRightLabel( TLabel label )
{
	DEBUG_PRINT( __FUNCTION__ )
	addLabelOperation( OT_MatchRightLabel, label );
}

void COperationsBuilder::AddMatchRightNumber( TNumber number )
{
	DEBUG_PRINT( __FUNCTION__ )
	addNumberOperation( OT_MatchRightNumber, number );
}

void COperationsBuilder::AddMatchLeftParens()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_MatchLeftParens );
}

void COperationsBuilder::AddMatchRightParens()
{
	DEBUG_PRINT( __FUNCTION__ )
	addNoArgumensOperation( OT_MatchRightParens );
}

void COperationsBuilder::AddMatchLeft_S( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_S : OT_MatchLeft_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_S :
			OT_MatchLeftWithQualifier_S ), qualifier );
	}
}

void COperationsBuilder::AddMatchRight_S( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_S : OT_MatchRight_S ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_S :
			 OT_MatchRightWithQualifier_S ), qualifier );
	}
}

void COperationsBuilder::AddMatchLeftDuplicate_S( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_S : OT_MatchLeftDuplicate_S ),
		tableIndex );
}


void COperationsBuilder::AddMatchRightDuplicate_S( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_S : OT_MatchRightDuplicate_S ),
		tableIndex );
}


void COperationsBuilder::AddMatchLeft_W( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchLeftSaveToTable_W : OT_MatchLeft_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchLeftWithQualifierSaveToTable_W :
			OT_MatchLeftWithQualifier_W ), qualifier );
	}
}

void COperationsBuilder::AddMatchRight_W( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchRightSaveToTable_W : OT_MatchRight_W ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchRightWithQualifierSaveToTable_W :
			 OT_MatchRightWithQualifier_W ), qualifier );
	}
}

void COperationsBuilder::AddMatchLeftDuplicate_WV( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_WV : OT_MatchLeftDuplicate_WV ),
		tableIndex );
}

void COperationsBuilder::AddMatchRightDuplicate_WV( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_WV : OT_MatchRightDuplicate_WV ),
		tableIndex );
}

void COperationsBuilder::AddMatchLeftDuplicate_E( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchLeftDuplicateSaveToTable_E : OT_MatchLeftDuplicate_E ),
		tableIndex );
}


void COperationsBuilder::AddMatchRightDuplicate_E( TTableIndex tableIndex,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addTableIndexOperation( ( saveInTable ?
		OT_MatchRightDuplicateSaveToTable_E : OT_MatchRightDuplicate_E ),
		tableIndex );
}

void COperationsBuilder::AddMatchClosed_V( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchClosedSaveToTable_V : OT_MatchClosed_V ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchClosedWithQualifierSaveToTable_V :
			 OT_MatchClosedWithQualifier_V ), qualifier );
	}
}

void COperationsBuilder::AddMatchClosed_E( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
		addNoArgumensOperation(
			( saveInTable ? OT_MatchClosedSaveToTable_E : OT_MatchClosed_E ) );
	} else {
		addQualifierIndexOperation(
			( saveInTable ? OT_MatchClosedWithQualifierSaveToTable_E :
			 OT_MatchClosedWithQualifier_E ), qualifier );
	}
}

/* matching V-variables by qualifier */
void COperationsBuilder::AddMacthLeftMaxByQualifier_V( CQualifier& qualifier, 
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_V :
		OT_MacthLeftMaxByQualifier_V ), qualifier );

}

void COperationsBuilder::AddMacthRightMaxByQualifier_V( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_V :
		OT_MacthRightMaxByQualifier_V ), qualifier );
}

/* matching E-variables by qualifier */
void COperationsBuilder::AddMacthLeftMaxByQualifier_E( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthLeftMaxByQualifierSaveToTable_E :
		OT_MacthLeftMaxByQualifier_E ), qualifier );
}

void COperationsBuilder::AddMacthRightMaxByQualifier_E( CQualifier& qualifier,
	const bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	addQualifierIndexOperation( ( saveInTable ?
		OT_MacthRightMaxByQualifierSaveToTable_E :
		OT_MacthRightMaxByQualifier_E ), qualifier );
}

/* match left VE-variable */
void COperationsBuilder::AddMatchLeft_E( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
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

void COperationsBuilder::AddMatchLeft_V( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
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
void COperationsBuilder::AddMatchRight_E( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
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

void COperationsBuilder::AddMatchRight_V( CQualifier& qualifier,
	bool saveInTable )
{
	DEBUG_PRINT( __FUNCTION__ )
	if( qualifier.IsEmpty() ) {
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
void COperationsBuilder::AddInsertChar( TChar c )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertChar )->c = c;
}

void COperationsBuilder::AddInsertLabel( TLabel label )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertLabel )->label = label;
}

void COperationsBuilder::AddInsertNumber( TNumber number )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertNumber )->number = number;
}

void COperationsBuilder::AddInsertLeftParen()
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertLeftParen );
}

void COperationsBuilder::AddInsertRightParen()
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertRightParen );
}

void COperationsBuilder::AddInsertRightBracket()
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_InsertRightBracket );
}

void COperationsBuilder::AddMove_S( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Move_S )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_S( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Copy_S )->tableIndex = tableIndex;
}

void COperationsBuilder::AddMove_E( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Move_E )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_E( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Copy_E )->tableIndex = tableIndex;
}

void COperationsBuilder::AddMove_WV( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Move_WV )->tableIndex = tableIndex;
}

void COperationsBuilder::AddCopy_WV( TTableIndex tableIndex )
{
	DEBUG_PRINT( __FUNCTION__ )
	addOperation( OT_Copy_WV )->tableIndex = tableIndex;
}

void COperationsBuilder::addNoArgumensOperation(
	const TOperationType type )
{
	addOperation( type );
}

void COperationsBuilder::addCharOperation( TOperationType type, TChar c )
{
	addOperation( type )->c = c;
}

void COperationsBuilder::addLabelOperation( TOperationType type, TLabel label )
{
	addOperation( type )->label = label;
}

void COperationsBuilder::addNumberOperation( TOperationType type,
	TNumber number )
{
	addOperation( type )->number = number;
}

void COperationsBuilder::addTableIndexOperation( TOperationType type,
	TTableIndex tableIndex )
{
	addOperation( type )->tableIndex = tableIndex;
}

void COperationsBuilder::addQualifierIndexOperation( TOperationType type,
	CQualifier& qualifier )
{
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void COperationsBuilder::addOperation_VE( TOperationType type )
{
	increaseStackDepth();
	addOperation( type );
}

void COperationsBuilder::addOperation_VE( TOperationType type,
	CQualifier& qualifier )
{
	increaseStackDepth();
	addOperation( type )->qualifier = registerQualifier( qualifier );
}

void COperationsBuilder::addStackDecrementOperation( TStackIndex size )
{
	decreaseStackDepth( size );
	addOperation( OT_DecrementStackDepth )->stackDecrement = size;
}

TQualifierIndex COperationsBuilder::registerQualifier( CQualifier& qualifier )
{
	static CQualifier* last = 0;
	static CQualifier* newQualifier = 0;
	if( last != &qualifier ) {
		newQualifier = new CQualifier;
		qualifier.Move( *newQualifier );
		last = &qualifier;
	}
	return newQualifier;
}

COperation* COperationsBuilder::addOperation( TOperationType type )
{
	addOperation( COperation( type ) );
	return operations.GetLast();
}

void COperationsBuilder::addOperation( const COperation& operation )
{
	operations.Append( operation );
}

void COperationsBuilder::increaseStackDepth()
{
	stackDepth++;
	if( stackDepth > maxRuleStackDepth ) {
		maxRuleStackDepth = stackDepth;
	}
}

void COperationsBuilder::decreaseStackDepth( TStackIndex size )
{
	stackDepth -= size;
	assert( stackDepth >= 0 );
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
