#include <iostream>
#include <Refal2.h>

namespace Refal2 {

COperationsExecuter::COperationsExecuter():
	left( 0 ),
	right( 0 ),
	tableTop( 0 ),
	operation( 0 ),
	stackTop( 0 ),
	lastAddedLeftParen( 0 ),
	lastAddedLeftBracket( 0 ),
	initialLeftBracket( CUnit( UT_LeftBracket ) )
{
}

static const char* operationNames[] = {
	"OT_Goto",
	"OT_InsertJump",
	"OT_MatchingComplete",
	"OT_Return",
	"OT_SetLeftBorder",
	"OT_SetRightBorder",
	"OT_DecrementStackDepth",
	"OT_MatchEmptyExpression",
	"OT_MatchLeftChar",
	"OT_MatchLeftLabel",
	"OT_MatchLeftNumber",
	"OT_MatchRightChar",
	"OT_MatchRightLabel",
	"OT_MatchRightNumber",
	"OT_MatchLeftParens",
	"OT_MatchRightParens",
	"OT_MatchLeft_S",
	"OT_MatchLeftSaveToTable_S",
	"OT_MatchLeftWithQualifier_S",
	"OT_MatchLeftWithQualifierSaveToTable_S",
	"OT_MatchRight_S",
	"OT_MatchRightSaveToTable_S",
	"OT_MatchRightWithQualifier_S",
	"OT_MatchRightWithQualifierSaveToTable_S",
	"OT_MatchLeftDuplicate_S",
	"OT_MatchLeftDuplicateSaveToTable_S",
	"OT_MatchRightDuplicate_S",
	"OT_MatchRightDuplicateSaveToTable_S",
	"OT_MatchLeft_W",
	"OT_MatchLeftSaveToTable_W",
	"OT_MatchLeftWithQualifier_W",
	"OT_MatchLeftWithQualifierSaveToTable_W",
	"OT_MatchRight_W",
	"OT_MatchRightSaveToTable_W",
	"OT_MatchRightWithQualifier_W",
	"OT_MatchRightWithQualifierSaveToTable_W",
	"OT_MatchLeftDuplicate_WV",
	"OT_MatchLeftDuplicateSaveToTable_WV",
	"OT_MatchRightDuplicate_WV",
	"OT_MatchRightDuplicateSaveToTable_WV",
	"OT_MatchLeftDuplicate_E",
	"OT_MatchLeftDuplicateSaveToTable_E",
	"OT_MatchRightDuplicate_E",
	"OT_MatchRightDuplicateSaveToTable_E",
	"OT_MatchClosed_V",
	"OT_MatchClosedSaveToTable_V",
	"OT_MatchClosedWithQualifier_V",
	"OT_MatchClosedWithQualifierSaveToTable_V",
	"OT_MatchClosed_E",
	"OT_MatchClosedSaveToTable_E",
	"OT_MatchClosedWithQualifier_E",
	"OT_MatchClosedWithQualifierSaveToTable_E",
	"OT_MacthLeftMaxByQualifier_V",
	"OT_MacthLeftMaxByQualifierSaveToTable_V",
	"OT_MacthRightMaxByQualifier_V",
	"OT_MacthRightMaxByQualifierSaveToTable_V",
	"OT_MacthLeftMaxByQualifier_E",
	"OT_MacthLeftMaxByQualifierSaveToTable_E",
	"OT_MacthRightMaxByQualifier_E",
	"OT_MacthRightMaxByQualifierSaveToTable_E",
	"OT_MatchLeftBegin_E",
	"OT_MatchLeftBeginSaveToTable_E",
	"OT_MatchLeftBegin_V",
	"OT_MatchLeftBeginSaveToTable_V",
	"OT_MatchLeftWithQulifierBegin_V",
	"OT_MatchLeftWithQulifierBeginSaveToTable_V",
	"OT_MatchLeft_E",
	"OT_MatchLeftSaveToTable_E",
	"OT_MatchLeftWithQulifier_E",
	"OT_MatchLeftWithQulifierSaveToTable_E",
	"OT_MatchRightBegin_E",
	"OT_MatchRightBeginSaveToTable_E",
	"OT_MatchRightBegin_V",
	"OT_MatchRightBeginSaveToTable_V",
	"OT_MatchRightWithQulifierBegin_V",
	"OT_MatchRightWithQulifierBeginSaveToTable_V",
	"OT_MatchRight_E",
	"OT_MatchRightSaveToTable_E",
	"OT_MatchRightWithQulifier_E",
	"OT_MatchRightWithQulifierSaveToTable_E",
	"OT_InsertChar",
	"OT_InsertLabel",
	"OT_InsertNumber",
	"OT_InsertLeftParen",
	"OT_InsertRightParen",
	"OT_InsertRightBracket",
	"OT_Move_S",
	"OT_Copy_S",
	"OT_Move_E",
	"OT_Copy_E",
	"OT_Move_WV",
	"OT_Copy_WV"
};

void COperationsExecuter::Run( const TLabel entry )
{
	fieldOfView.Empty();
	left = fieldOfView.Append( CUnit( UT_LeftBracket ) );
	left->PairedParen() = 0;
	fieldOfView.Append( CUnit( UT_Label ) )->Label() = entry;
	right = fieldOfView.Append( CUnit( UT_RightBracket ) );
	right->PairedParen() = left;
	initialLeftBracket.PairedParen() = right;

	while( initialLeftBracket.PairedParen() != 0 ) {
		right = initialLeftBracket.PairedParen();
		left = right->PairedParen()->Next();
		operation = static_cast<COperationNode*>(
			LabelTable.GetLabelFunction( left->Label() )->firstOperation );
		tableTop = 0;
		stackTop = 0;
		lastAddedLeftParen = 0;
		lastAddedLeftBracket = &initialLeftBracket;
		initialLeftBracket.PairedParen() = 0;
		saveToTable( left, right );
		while( !doOperation() ) {
			nextOperation();
		}
	}

	std::cout << "\n\n\n-----------------------------------------\n\n";
	PrintUnitList( fieldOfView );
	std::cout << "\n\n-----------------------------------------\n\n";
}

bool COperationsExecuter::doOperation()
{
	std::cout << operationNames[operation->type] << "\n";
	switch( operation->type ) {
		case OT_Goto: /* TOperationAddress */
			assert( false );
			break;
		case OT_InsertJump: /* TOperationAddress */
			insertJump( operation->operation );
			break;
		case OT_MatchingComplete:
			matchingComplete();
			break;
		case OT_Return:
			doReturn();
			return true;
			break;
		case OT_SetLeftBorder: /* TTableIndex */
			setLeftBorder( operation->tableIndex );
			break;
		case OT_SetRightBorder: /* TTableIndex */
			setRightBorder( operation->tableIndex );
			break;
		case OT_DecrementStackDepth: /* TUint32 */
			assert( false );
			break;
		/* matching empty expression */
		case OT_MatchEmptyExpression:
			matchEmptyExpression();
			break;
		/* matching symbols */
		case OT_MatchLeftChar: /* TChar */
			matchLeftChar( operation->c );
			break;
		case OT_MatchLeftLabel: /* TLabel */
			matchLeftLabel( operation->label );
			break;
		case OT_MatchLeftNumber: /* TNumber */
			matchLeftNumber( operation->number );
			break;
		case OT_MatchRightChar: /* TChar */
			matchRightChar( operation->c );
			break;
		case OT_MatchRightLabel: /* TLabel */
			matchRightLabel( operation->label );
			break;
		case OT_MatchRightNumber: /* TNumber */
			matchRightNumber( operation->number );
			break;
		/* matching parens */
		case OT_MatchLeftParens:
			matchLeftParens();
			break;
		case OT_MatchRightParens:
			matchRightParens();
			break;
		/* matching S-variables */
		case OT_MatchLeft_S:
			matchLeft_S();
			break;
		case OT_MatchLeftSaveToTable_S:
			matchLeftSaveToTable_S();
			break;
		case OT_MatchLeftWithQualifier_S: /* TQualifierIndex */
			matchLeftWithQualifier_S( operation->qualifier );
			break;
		case OT_MatchLeftWithQualifierSaveToTable_S: /* TQualifierIndex */
			matchLeftWithQualifierSaveToTable_S( operation->qualifier );
			break;
		case OT_MatchRight_S:
			matchRight_S();
			break;
		case OT_MatchRightSaveToTable_S:
			matchRightSaveToTable_S();
			break;
		case OT_MatchRightWithQualifier_S: /* TQualifierIndex */
			matchRightWithQualifier_S( operation->qualifier );
			break;
		case OT_MatchRightWithQualifierSaveToTable_S: /* TQualifierIndex */
			matchRightWithQualifierSaveToTable_S( operation->qualifier );
			break;
		/* matching duplicate of S-variables */
		case OT_MatchLeftDuplicate_S: /* TTableIndex */
			matchLeftDuplicate_S( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_S: /* TTableIndex */
			matchLeftDuplicateSaveToTable_S( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_S: /* TTableIndex */
			matchRightDuplicate_S( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_S: /* TTableIndex */
			matchRightDuplicateSaveToTable_S( operation->tableIndex );
			break;
		/* matching W-variables */
		case OT_MatchLeft_W:
			matchLeft_W();
			break;
		case OT_MatchLeftSaveToTable_W:
			matchLeftSaveToTable_W();
			break;
		case OT_MatchLeftWithQualifier_W: /* TQualifierIndex */
			matchLeftWithQualifier_W( operation->qualifier );
			break;
		case OT_MatchLeftWithQualifierSaveToTable_W: /* TQualifierIndex */
			matchLeftWithQualifierSaveToTable_W( operation->qualifier );
			break;
		case OT_MatchRight_W:
			matchRight_W();
			break;
		case OT_MatchRightSaveToTable_W:
			matchRightSaveToTable_W();
			break;
		case OT_MatchRightWithQualifier_W: /* TQualifierIndex */
			matchRightWithQualifier_W( operation->qualifier );
			break;
		case OT_MatchRightWithQualifierSaveToTable_W: /* TQualifierIndex */
			matchRightWithQualifierSaveToTable_W( operation->qualifier );
			break;
		/* matching duplicate of WV-variables */
		case OT_MatchLeftDuplicate_WV: /* TTableIndex */
			matchLeftDuplicate_WV( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_WV: /* TTableIndex */
			matchLeftDuplicateSaveToTable_WV( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_WV: /* TTableIndex */
			matchRightDuplicate_WV( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_WV: /* TTableIndex */
			matchRightDuplicateSaveToTable_WV( operation->tableIndex );
			break;
		/* matching duplicate of E-variables */
		case OT_MatchLeftDuplicate_E: /* TTableIndex */
			matchLeftDuplicate_E( operation->tableIndex );
			break;
		case OT_MatchLeftDuplicateSaveToTable_E: /* TTableIndex */
			matchLeftDuplicateSaveToTable_E( operation->tableIndex );
			break;
		case OT_MatchRightDuplicate_E: /* TTableIndex */
			matchRightDuplicate_E( operation->tableIndex );
			break;
		case OT_MatchRightDuplicateSaveToTable_E: /* TTableIndex */
			matchRightDuplicateSaveToTable_E( operation->tableIndex );
			break;
		/* matching closed V-variables */
		case OT_MatchClosed_V:
			matchClosed_V();
			break;
		case OT_MatchClosedSaveToTable_V:
			matchClosedSaveToTable_V();
			break;
		case OT_MatchClosedWithQualifier_V: /* TQualifierIndex */
			matchClosedWithQualifier_V( operation->qualifier );
			break;
		case OT_MatchClosedWithQualifierSaveToTable_V: /* TQualifierIndex */
			matchClosedWithQualifierSaveToTable_V( operation->qualifier );
			break;
		/* matching closed E-variables */
		case OT_MatchClosed_E:
			matchClosed_E();
			break;
		case OT_MatchClosedSaveToTable_E:
			matchClosedSaveToTable_E();
			break;
		case OT_MatchClosedWithQualifier_E: /* TQualifierIndex */
			matchClosedWithQualifier_E( operation->qualifier );
			break;
		case OT_MatchClosedWithQualifierSaveToTable_E: /* TQualifierIndex */
			matchClosedWithQualifierSaveToTable_E( operation->qualifier );
			break;
		/* matching V-variables by qualifier */
		case OT_MacthLeftMaxByQualifier_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthLeftMaxByQualifierSaveToTable_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifier_V: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifierSaveToTable_V: /* TQualifierIndex */
			assert( false );
			break;
		/* matching E-variables by qualifier */
		case OT_MacthLeftMaxByQualifier_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthLeftMaxByQualifierSaveToTable_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifier_E: /* TQualifierIndex */
			assert( false );
			break;
		case OT_MacthRightMaxByQualifierSaveToTable_E: /* TQualifierIndex */
			assert( false );
			break;
		/* match left VE-variable */
		case OT_MatchLeftBegin_E:
			matchLeftBegin_E();
			break;
		case OT_MatchLeftBeginSaveToTable_E:
			matchLeftBeginSaveToTable_E();
			break;
		case OT_MatchLeftBegin_V:
			matchLeftBegin_V();
			break;
		case OT_MatchLeftBeginSaveToTable_V:
			matchLeftBeginSaveToTable_V();
			break;
		case OT_MatchLeftWithQulifierBegin_V: /* TQualifierIndex */
			matchLeftWithQulifierBegin_V( operation->qualifier );
			break;
		case OT_MatchLeftWithQulifierBeginSaveToTable_V: /* TQualifierIndex */
			matchLeftWithQulifierBeginSaveToTable_V( operation->qualifier );
			break;
		case OT_MatchLeft_E:
			matchLeft_E();
			break;
		case OT_MatchLeftSaveToTable_E:
			matchLeftSaveToTable_E();
			break;
		case OT_MatchLeftWithQulifier_E: /* TQualifierIndex */
			matchLeftWithQulifier_E( operation->qualifier );
			break;
		case OT_MatchLeftWithQulifierSaveToTable_E: /* TQualifierIndex */
			matchLeftWithQulifierSaveToTable_E( operation->qualifier );
			break;
		/* match right VE-variable */
		case OT_MatchRightBegin_E:
			matchRightBegin_E();
			break;
		case OT_MatchRightBeginSaveToTable_E:
			matchRightBeginSaveToTable_E();
			break;
		case OT_MatchRightBegin_V:
			matchRightBegin_V();
			break;
		case OT_MatchRightBeginSaveToTable_V:
			matchRightBeginSaveToTable_V();
			break;
		case OT_MatchRightWithQulifierBegin_V: /* TQualifierIndex */
			matchRightWithQulifierBegin_V( operation->qualifier );
			break;
		case OT_MatchRightWithQulifierBeginSaveToTable_V: /* TQualifierIndex */
			matchRightWithQulifierBeginSaveToTable_V( operation->qualifier );
			break;
		case OT_MatchRight_E:
			matchRight_E();
			break;
		case OT_MatchRightSaveToTable_E:
			matchRightSaveToTable_E();
			break;
		case OT_MatchRightWithQulifier_E: /* TQualifierIndex */
			matchRightWithQulifier_E( operation->qualifier );
			break;
		case OT_MatchRightWithQulifierSaveToTable_E: /* TQualifierIndex */
			matchRightWithQulifierSaveToTable_E( operation->qualifier );
			break;
		/* making operations */
		case OT_InsertChar: /* TChar */
			insertChar( operation->c );
			break;
		case OT_InsertLabel: /* TLabel */
			insertLabel( operation->label );
			break;
		case OT_InsertNumber: /* TNumber */
			insertNumber( operation->number );
			break;
		case OT_InsertLeftParen:
			insertLeftParen();
			break;
		case OT_InsertRightParen:
			insertRightParen();
			break;
		case OT_InsertRightBracket:
			insertRightBracket();
			break;
		case OT_Move_S: /* TTableIndex */
			move_S( operation->tableIndex );
			break;
		case OT_Copy_S: /* TTableIndex */
			copy_S( operation->tableIndex );
			break;
		case OT_Move_E: /* TTableIndex */
			move_E( operation->tableIndex );
			break;
		case OT_Copy_E: /* TTableIndex */
			copy_E( operation->tableIndex );
			break;
		case OT_Move_WV: /* TTableIndex */
			move_WV( operation->tableIndex );
			break;
		case OT_Copy_WV: /* TTableIndex */
			copy_WV( operation->tableIndex );
			break;
		default:
			assert( false );
			break;
	}
	return false;
}

} // end of namespace refal2