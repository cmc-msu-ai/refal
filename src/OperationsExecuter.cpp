#include <Refal2.h>

namespace Refal2 {

static const char* operationNames[] = {
	"OT_Goto",
	"OT_InsertJump",
	"OT_MatchingComplete",
	"OT_Return",
	"OT_SetLeftBorder",
	"OT_SetRightBorder",
	"OT_DecrementStackDepth",
	"OT_SaveLeftRight",
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

TExecutionResult COperationsExecuter::Run( CProgramPtr program,
	CReceptaclePtr receptacle, CUnitList& fieldOfView, CUnitNode*& errorCall )
{
	COperationsExecuter executer( program, receptacle );
	executer.fieldOfView.Move( fieldOfView );
	errorCall = executer.initialLeftBracket.PairedParen();
	return executer.executionResult;
}

COperationsExecuter::COperationsExecuter( CProgramPtr program,
		CReceptaclePtr receptacle ) :
	CExecutionContext( program, receptacle ),
	executionResult( ER_OK ),
	left( 0 ),
	right( 0 ),
	tableTop( 0 ),
	operation( nullptr ),
	stackTop( 0 ),
	initialLeftBracket( CUnit( UT_LeftBracket ) ),
	lastAddedLeftParen( nullptr ),
	lastAddedLeftBracket( nullptr )
{
	allocateTableAndStack();

	// add <`ProgramStartFunctionName`> to fieldOfVirw
	left = fieldOfView.Append( CUnit( UT_LeftParen ) );
	left->PairedParen() = nullptr;

	fieldOfView.Append( CUnit( UT_Label ) )->Label() =
		program->GetProgramStartFunction();
	right = fieldOfView.Append( CUnit( UT_RightBracket ) );
	right->PairedParen() = left;

	// start execution
	initialLeftBracket.PairedParen() = right;
	while( initialLeftBracket.PairedParen() != 0 ) {
		doFunction();
		if( executionResult != ER_OK ) {
			break;
		}
	}
	restoreLeftBrackets();
}

COperationsExecuter::~COperationsExecuter()
{
	::operator delete( stack );
}

void COperationsExecuter::allocateTableAndStack()
{
	assert( Program->MaxTableSize() >= 2 );
	const int stackBytes = sizeof( CStackData ) * Program->MaxStackDepth();
	const int tableBytes = sizeof( CUnitNode* ) * Program->MaxTableSize();
	void* memory = ::operator new( stackBytes + tableBytes );
	stack = static_cast<CStackData*>( memory );
	memory = static_cast<char*>( memory ) + stackBytes;
	table = static_cast<CUnitNode**>( memory );
}

void COperationsExecuter::restoreLeftBrackets()
{
	CUnitNode* rightBracket = initialLeftBracket.PairedParen();
	while( rightBracket != 0 ) {
		rightBracket = rightBracket->PairedParen();
		rightBracket->SetType( UT_LeftBracket );
		rightBracket = rightBracket->PairedParen();
	}
}

void COperationsExecuter::doFunction()
{
	CUnitNode* savedRightBracket = initialLeftBracket.PairedParen();
	right = savedRightBracket;
	left = right->PairedParen();
	CUnitNode* savedNextRightBracket = left->PairedParen();
	left = left->Next();
	if( left->IsLabel() ) {
		RuntimeModuleId = left->Label() / LabelMask;
		const TLabel functionIndex = left->Label() % LabelMask;
		const CRuntimeFunctions& functions = Program->Module(
			RuntimeModuleId ).Functions;
		const CRuntimeFunction* function = functions.GetData(
			functionIndex ).get();
		DEBUG_PRINT( __FUNCTION__ << " " << functions.GetKey( functionIndex ) )
		if( function->IsOrdinary() ) {
			const COrdinaryFunction* ordinaryFunction =
				static_cast<const COrdinaryFunction*>( function );
			operation = static_cast<COperationNode*>(
				ordinaryFunction->FirstOperation() );
			RuntimeModuleId = ordinaryFunction->RuntimeModuleId();
			tableTop = 0;
			stackTop = 0;
			lastAddedLeftParen = 0;
			lastAddedLeftBracket = &initialLeftBracket;
			initialLeftBracket.PairedParen() = 0;
			saveToTable( left, right );
			doFunctionBody();
		} else if( function->IsEmbedded() ) {
			const CEmbeddedFunction* embeddedFunction =
				static_cast<const CEmbeddedFunction*>( function );
			if( left->Next() != right ) {
				CUnitNode* argumentBegin = left->Next();
				CUnitNode* argumentEnd = right->Prev();
				fieldOfView.Detach( argumentBegin, argumentEnd );
				Argument.Assign( argumentBegin, argumentEnd );
			}
			if( embeddedFunction->EmbeddedFunction()( *this ) ) {
				if( !Argument.IsEmpty() ) {
					CUnitNode* argumentBegin = Argument.GetFirst();
					CUnitNode* argumentEnd = Argument.GetLast();
					Argument.Detach();
					fieldOfView.InsertAfter( right, argumentBegin, argumentEnd );
				}
				fieldOfView.Remove( left->Prev(), right );
				lastAddedLeftParen = 0;
				lastAddedLeftBracket = &initialLeftBracket;
				initialLeftBracket.PairedParen() = 0;
			} else {
				executionResult = ER_WrongArgumentOfExternalFunction;
				fieldOfView.InsertAfter( left, Argument );
			}
		} else if( function->IsEmpty() ) {
			executionResult = ER_CallEmptyFunction;
		} else {
			assert( false );
		}
	} else {
		executionResult = ER_LostFunctionLabel;
	}
	if( executionResult == ER_OK ) {
		lastAddedLeftBracket->PairedParen() = savedNextRightBracket;
	} else {
		initialLeftBracket.PairedParen() = savedRightBracket;
	}
}

void COperationsExecuter::doFunctionBody()
{
	while( true ) {
		DEBUG_PRINT( operationNames[operation->type] )
		bool success = true;
		switch( operation->type ) {
			case OT_Goto: // TOperationAddress
				assert( false );
				break;
			case OT_InsertJump: // TOperationAddress
				insertJump( operation->operation );
				break;
			case OT_MatchingComplete:
				matchingComplete();
				break;
			case OT_Return:
				doReturn();
				return;
				break;
			case OT_SetLeftBorder: // TTableIndex
				setLeftBorder( operation->tableIndex );
				break;
			case OT_SetRightBorder: // TTableIndex
				setRightBorder( operation->tableIndex );
				break;
			case OT_DecrementStackDepth: // TUint32
				decrementStackDepth( operation->stackDecrement );
				break;
			case OT_SaveLeftRight:
				saveToTable( left, right );
				break;
			// matching empty expression
			case OT_MatchEmptyExpression:
				success = matchEmptyExpression();
				break;
			// matching symbols
			case OT_MatchLeftChar: // TChar
				success = matchLeftChar( operation->c );
				break;
			case OT_MatchLeftLabel: // TLabel
				success = matchLeftLabel( operation->label );
				break;
			case OT_MatchLeftNumber: // TNumber
				success = matchLeftNumber( operation->number );
				break;
			case OT_MatchRightChar: // TChar
				success = matchRightChar( operation->c );
				break;
			case OT_MatchRightLabel: // TLabel
				success = matchRightLabel( operation->label );
				break;
			case OT_MatchRightNumber: // TNumber
				success = matchRightNumber( operation->number );
				break;
			// matching parens
			case OT_MatchLeftParens:
				success = matchLeftParens();
				break;
			case OT_MatchRightParens:
				success = matchRightParens();
				break;
			// matching S-variables
			case OT_MatchLeft_S:
				success = matchLeft_S();
				break;
			case OT_MatchLeftSaveToTable_S:
				success = matchLeftSaveToTable_S();
				break;
			case OT_MatchLeftWithQualifier_S: // TQualifierIndex
				success = matchLeftWithQualifier_S( operation->qualifier );
				break;
			case OT_MatchLeftWithQualifierSaveToTable_S: // TQualifierIndex
				success = matchLeftWithQualifierSaveToTable_S(
					operation->qualifier );
				break;
			case OT_MatchRight_S:
				success = matchRight_S();
				break;
			case OT_MatchRightSaveToTable_S:
				success = matchRightSaveToTable_S();
				break;
			case OT_MatchRightWithQualifier_S: // TQualifierIndex
				success = matchRightWithQualifier_S( operation->qualifier );
				break;
			case OT_MatchRightWithQualifierSaveToTable_S: // TQualifierIndex
				success = matchRightWithQualifierSaveToTable_S(
					operation->qualifier );
				break;
			// matching duplicate of S-variables
			case OT_MatchLeftDuplicate_S: // TTableIndex
				success = matchLeftDuplicate_S( operation->tableIndex );
				break;
			case OT_MatchLeftDuplicateSaveToTable_S: // TTableIndex
				success = matchLeftDuplicateSaveToTable_S(
					operation->tableIndex );
				break;
			case OT_MatchRightDuplicate_S: // TTableIndex
				success = matchRightDuplicate_S( operation->tableIndex );
				break;
			case OT_MatchRightDuplicateSaveToTable_S: // TTableIndex
				success = matchRightDuplicateSaveToTable_S(
					operation->tableIndex );
				break;
			// matching W-variables
			case OT_MatchLeft_W:
				success = matchLeft_W();
				break;
			case OT_MatchLeftSaveToTable_W:
				success = matchLeftSaveToTable_W();
				break;
			case OT_MatchLeftWithQualifier_W: // TQualifierIndex
				success = matchLeftWithQualifier_W( operation->qualifier );
				break;
			case OT_MatchLeftWithQualifierSaveToTable_W: // TQualifierIndex
				success = matchLeftWithQualifierSaveToTable_W(
					operation->qualifier );
				break;
			case OT_MatchRight_W:
				success = matchRight_W();
				break;
			case OT_MatchRightSaveToTable_W:
				success = matchRightSaveToTable_W();
				break;
			case OT_MatchRightWithQualifier_W: // TQualifierIndex
				success = matchRightWithQualifier_W( operation->qualifier );
				break;
			case OT_MatchRightWithQualifierSaveToTable_W: // TQualifierIndex
				success = matchRightWithQualifierSaveToTable_W(
					operation->qualifier );
				break;
			// matching duplicate of WV-variables
			case OT_MatchLeftDuplicate_WV: // TTableIndex
				success = matchLeftDuplicate_WV( operation->tableIndex );
				break;
			case OT_MatchLeftDuplicateSaveToTable_WV: // TTableIndex
				success = matchLeftDuplicateSaveToTable_WV(
					operation->tableIndex );
				break;
			case OT_MatchRightDuplicate_WV: // TTableIndex
				success = matchRightDuplicate_WV( operation->tableIndex );
				break;
			case OT_MatchRightDuplicateSaveToTable_WV: // TTableIndex
				success = matchRightDuplicateSaveToTable_WV(
					operation->tableIndex );
				break;
			// matching duplicate of E-variables
			case OT_MatchLeftDuplicate_E: // TTableIndex
				success = matchLeftDuplicate_E( operation->tableIndex );
				break;
			case OT_MatchLeftDuplicateSaveToTable_E: // TTableIndex
				success = matchLeftDuplicateSaveToTable_E(
					operation->tableIndex );
				break;
			case OT_MatchRightDuplicate_E: // TTableIndex
				success = matchRightDuplicate_E( operation->tableIndex );
				break;
			case OT_MatchRightDuplicateSaveToTable_E: // TTableIndex
				success = matchRightDuplicateSaveToTable_E(
					operation->tableIndex );
				break;
			// matching closed V-variables
			case OT_MatchClosed_V:
				success = matchClosed_V();
				break;
			case OT_MatchClosedSaveToTable_V:
				success = matchClosedSaveToTable_V();
				break;
			case OT_MatchClosedWithQualifier_V: // TQualifierIndex
				success = matchClosedWithQualifier_V( operation->qualifier );
				break;
			case OT_MatchClosedWithQualifierSaveToTable_V: // TQualifierIndex
				success = matchClosedWithQualifierSaveToTable_V(
					operation->qualifier );
				break;
			// matching closed E-variables
			case OT_MatchClosed_E:
				matchClosed_E();
				break;
			case OT_MatchClosedSaveToTable_E:
				matchClosedSaveToTable_E();
				break;
			case OT_MatchClosedWithQualifier_E: // TQualifierIndex
				success = matchClosedWithQualifier_E( operation->qualifier );
				break;
			case OT_MatchClosedWithQualifierSaveToTable_E: // TQualifierIndex
				success = matchClosedWithQualifierSaveToTable_E(
					operation->qualifier );
				break;
			// matching V-variables by qualifier
			case OT_MacthLeftMaxByQualifier_V: // TQualifierIndex
				success = macthLeftMaxByQualifier_V( operation->qualifier );
				break;
			case OT_MacthLeftMaxByQualifierSaveToTable_V: // TQualifierIndex
				success = macthLeftMaxByQualifierSaveToTable_V(
					operation->qualifier );
				break;
			case OT_MacthRightMaxByQualifier_V: // TQualifierIndex
				success = macthRightMaxByQualifier_V( operation->qualifier );
				break;
			case OT_MacthRightMaxByQualifierSaveToTable_V: // TQualifierIndex
				success = macthRightMaxByQualifierSaveToTable_V(
					operation->qualifier );
				break;
			// matching E-variables by qualifier
			case OT_MacthLeftMaxByQualifier_E: // TQualifierIndex
				macthLeftMaxByQualifier_E( operation->qualifier );
				break;
			case OT_MacthLeftMaxByQualifierSaveToTable_E: // TQualifierIndex
				macthLeftMaxByQualifierSaveToTable_E( operation->qualifier );
				break;
			case OT_MacthRightMaxByQualifier_E: // TQualifierIndex
				macthRightMaxByQualifier_E( operation->qualifier );
				break;
			case OT_MacthRightMaxByQualifierSaveToTable_E: // TQualifierIndex
				macthRightMaxByQualifierSaveToTable_E( operation->qualifier );
				break;
			// match left VE-variable
			case OT_MatchLeftBegin_E:
				matchLeftBegin_E();
				break;
			case OT_MatchLeftBeginSaveToTable_E:
				matchLeftBeginSaveToTable_E();
				break;
			case OT_MatchLeftBegin_V:
				success = matchLeftBegin_V();
				break;
			case OT_MatchLeftBeginSaveToTable_V:
				success = matchLeftBeginSaveToTable_V();
				break;
			case OT_MatchLeftWithQulifierBegin_V: // TQualifierIndex
				success = matchLeftWithQulifierBegin_V( operation->qualifier );
				break;
			case OT_MatchLeftWithQulifierBeginSaveToTable_V: // TQualifierIndex
				success = matchLeftWithQulifierBeginSaveToTable_V(
					operation->qualifier );
				break;
			case OT_MatchLeft_E:
				success = matchLeft_E();
				break;
			case OT_MatchLeftSaveToTable_E:
				success = matchLeftSaveToTable_E();
				break;
			case OT_MatchLeftWithQulifier_E: // TQualifierIndex
				success = matchLeftWithQulifier_E( operation->qualifier );
				break;
			case OT_MatchLeftWithQulifierSaveToTable_E: // TQualifierIndex
				success = matchLeftWithQulifierSaveToTable_E(
					operation->qualifier );
				break;
			// match right VE-variable
			case OT_MatchRightBegin_E:
				matchRightBegin_E();
				break;
			case OT_MatchRightBeginSaveToTable_E:
				matchRightBeginSaveToTable_E();
				break;
			case OT_MatchRightBegin_V:
				success = matchRightBegin_V();
				break;
			case OT_MatchRightBeginSaveToTable_V:
				success = matchRightBeginSaveToTable_V();
				break;
			case OT_MatchRightWithQulifierBegin_V: // TQualifierIndex
				success = matchRightWithQulifierBegin_V( operation->qualifier );
				break;
			case OT_MatchRightWithQulifierBeginSaveToTable_V: // TQualifierIndex
				success = matchRightWithQulifierBeginSaveToTable_V(
					operation->qualifier );
				break;
			case OT_MatchRight_E:
				success = matchRight_E();
				break;
			case OT_MatchRightSaveToTable_E:
				success = matchRightSaveToTable_E();
				break;
			case OT_MatchRightWithQulifier_E: // TQualifierIndex
				success = matchRightWithQulifier_E( operation->qualifier );
				break;
			case OT_MatchRightWithQulifierSaveToTable_E: // TQualifierIndex
				success = matchRightWithQulifierSaveToTable_E(
					operation->qualifier );
				break;
			// making operations
			case OT_InsertChar: // TChar
				insertChar( operation->c );
				break;
			case OT_InsertLabel: // TLabel
				insertLabel( operation->label );
				break;
			case OT_InsertNumber: // TNumber
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
			case OT_Move_S: // TTableIndex
				move_S( operation->tableIndex );
				break;
			case OT_Copy_S: // TTableIndex
				copy_S( operation->tableIndex );
				break;
			case OT_Move_E: // TTableIndex
				move_E( operation->tableIndex );
				break;
			case OT_Copy_E: // TTableIndex
				copy_E( operation->tableIndex );
				break;
			case OT_Move_WV: // TTableIndex
				move_WV( operation->tableIndex );
				break;
			case OT_Copy_WV: // TTableIndex
				copy_WV( operation->tableIndex );
				break;
			default:
				assert( false );
				break;
		}
		if( success ) {
			nextOperation();
		} else {
			if( !fail() ) {
				executionResult = ER_RecognitionImpossible;
				return;
			}
		}
	}
	assert( false );
}

} // end of namespace refal2
