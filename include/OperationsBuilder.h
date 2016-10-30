#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

typedef COperation* TOperationAddress;
typedef CQualifier* TQualifierIndex;

enum TOperationType {
	OT_Goto, // TOperationAddress
	OT_InsertJump, // TOperationAddress
	OT_MatchingComplete,
	OT_Return,
	OT_SetLeftBorder, // TTableIndex
	OT_SetRightBorder, // TTableIndex
	OT_DecrementStackDepth, // TStackIndex
	OT_SaveLeftRight,
	// matching empty expression
	OT_MatchEmptyExpression,
	// matching symbols
	OT_MatchLeftChar, // TChar
	OT_MatchLeftLabel, // TLabel
	OT_MatchLeftNumber, // TNumber
	OT_MatchRightChar, // TChar
	OT_MatchRightLabel, // TLabel
	OT_MatchRightNumber, // TNumber
	// matching parens
	OT_MatchLeftParens,
	OT_MatchRightParens,
	// matching S-variables
	OT_MatchLeft_S,
	OT_MatchLeftSaveToTable_S,
	OT_MatchLeftWithQualifier_S, // TQualifierIndex
	OT_MatchLeftWithQualifierSaveToTable_S, // TQualifierIndex
	OT_MatchRight_S,
	OT_MatchRightSaveToTable_S,
	OT_MatchRightWithQualifier_S, // TQualifierIndex
	OT_MatchRightWithQualifierSaveToTable_S, // TQualifierIndex
	// matching duplicate of S-variables
	OT_MatchLeftDuplicate_S, // TTableIndex
	OT_MatchLeftDuplicateSaveToTable_S, // TTableIndex
	OT_MatchRightDuplicate_S, // TTableIndex
	OT_MatchRightDuplicateSaveToTable_S, // TTableIndex
	// matching W-variables
	OT_MatchLeft_W,
	OT_MatchLeftSaveToTable_W,
	OT_MatchLeftWithQualifier_W, // TQualifierIndex
	OT_MatchLeftWithQualifierSaveToTable_W, // TQualifierIndex
	OT_MatchRight_W,
	OT_MatchRightSaveToTable_W,
	OT_MatchRightWithQualifier_W, // TQualifierIndex
	OT_MatchRightWithQualifierSaveToTable_W, // TQualifierIndex
	// matching duplicate of WV-variables
	OT_MatchLeftDuplicate_WV, // TTableIndex
	OT_MatchLeftDuplicateSaveToTable_WV, // TTableIndex
	OT_MatchRightDuplicate_WV, // TTableIndex
	OT_MatchRightDuplicateSaveToTable_WV, // TTableIndex
	// matching duplicate of E-variables
	OT_MatchLeftDuplicate_E, // TTableIndex
	OT_MatchLeftDuplicateSaveToTable_E, // TTableIndex
	OT_MatchRightDuplicate_E, // TTableIndex
	OT_MatchRightDuplicateSaveToTable_E, // TTableIndex
	// matching closed V-variables
	OT_MatchClosed_V,
	OT_MatchClosedSaveToTable_V,
	OT_MatchClosedWithQualifier_V, // TQualifierIndex
	OT_MatchClosedWithQualifierSaveToTable_V, // TQualifierIndex
	// matching closed E-variables
	OT_MatchClosed_E,
	OT_MatchClosedSaveToTable_E,
	OT_MatchClosedWithQualifier_E, // TQualifierIndex
	OT_MatchClosedWithQualifierSaveToTable_E, // TQualifierIndex
	// matching V-variables by qualifier
	OT_MacthLeftMaxByQualifier_V, // TQualifierIndex
	OT_MacthLeftMaxByQualifierSaveToTable_V, // TQualifierIndex
	OT_MacthRightMaxByQualifier_V, // TQualifierIndex
	OT_MacthRightMaxByQualifierSaveToTable_V, // TQualifierIndex
	// matching E-variables by qualifier
	OT_MacthLeftMaxByQualifier_E, // TQualifierIndex
	OT_MacthLeftMaxByQualifierSaveToTable_E, // TQualifierIndex
	OT_MacthRightMaxByQualifier_E, // TQualifierIndex
	OT_MacthRightMaxByQualifierSaveToTable_E, // TQualifierIndex
	// match left VE-variable
	OT_MatchLeftBegin_E,
	OT_MatchLeftBeginSaveToTable_E,
	OT_MatchLeftBegin_V,
	OT_MatchLeftBeginSaveToTable_V,
	OT_MatchLeftWithQulifierBegin_V, // TQualifierIndex
	OT_MatchLeftWithQulifierBeginSaveToTable_V, // TQualifierIndex
	OT_MatchLeft_E,
	OT_MatchLeftSaveToTable_E,
	OT_MatchLeftWithQulifier_E, // TQualifierIndex
	OT_MatchLeftWithQulifierSaveToTable_E, // TQualifierIndex
	// match right VE-variable
	OT_MatchRightBegin_E,
	OT_MatchRightBeginSaveToTable_E,
	OT_MatchRightBegin_V,
	OT_MatchRightBeginSaveToTable_V,
	OT_MatchRightWithQulifierBegin_V, // TQualifierIndex
	OT_MatchRightWithQulifierBeginSaveToTable_V, // TQualifierIndex
	OT_MatchRight_E,
	OT_MatchRightSaveToTable_E,
	OT_MatchRightWithQulifier_E, // TQualifierIndex
	OT_MatchRightWithQulifierSaveToTable_E, // TQualifierIndex
	// making operations
	OT_InsertChar, // TChar
	OT_InsertLabel, // TLabel
	OT_InsertNumber, // TNumber
	OT_InsertLeftParen,
	OT_InsertRightParen,
	OT_InsertRightBracket,
	OT_Move_S, // TTableIndex
	OT_Copy_S, // TTableIndex
	OT_Move_E, // TTableIndex
	OT_Copy_E, // TTableIndex
	OT_Move_WV, // TTableIndex
	OT_Copy_WV // TTableIndex
};

/*
EX    = OT_MatchLeftBegin_E; OT_MatchLeftBegin_V; OT_MatchLeft_E;
E:Q:X = OT_MatchLeftBegin_E; OT_MatchLeftWithQulifierBegin_V; OT_MatchLeftWithQulifier_E;
VX    = OT_MatchLeftBegin_V; OT_MatchLeft_E;
V:Q:X = OT_MatchLeftWithQulifierBegin_V; OT_MatchLeftWithQulifier_E;
*/

//-----------------------------------------------------------------------------
// COperation

class COperation {
	friend class COperationsBuilder;
	friend class COperationsExecuter;

public:
	COperation() {}
	
private:
	explicit COperation( const TOperationType _type ): type( _type ) {}
	
	TOperationType type;
	union {
		TChar c;
		TLabel label;
		TNumber number;
		TStackIndex stackDecrement;
		TTableIndex tableIndex;
		TQualifierIndex qualifier;
		TOperationAddress operation;
	};
};

//-----------------------------------------------------------------------------
// COperationsBuilder

typedef CNodeList<COperation> COperationList;
typedef COperationList::CNodeType COperationNode;

class COperationsBuilder {
public:
	COperationsBuilder() { Reset(); }

	void Reset();
	void Export( COperationList& saveTo );

	void AddMatchingComplete();
	void AddReturn();
	void AddDecrementStackDepth( TStackIndex size );
	void AddSetLeftBorder( TTableIndex );
	void AddSetRightBorder( TTableIndex );
	void AddSaveLeftRight();
	// matching empty expression
	void AddMatchEmptyExpression();
	// matching symbols
	void AddMatchLeftChar( TChar c );
	void AddMatchLeftLabel( TLabel label );
	void AddMatchLeftNumber( TNumber number );
	void AddMatchRightChar( TChar c );
	void AddMatchRightLabel( TLabel label );
	void AddMatchRightNumber( TNumber number );
	// matching parens
	void AddMatchLeftParens();
	void AddMatchRightParens();
	// matching S-variables
	void AddMatchLeft_S( CQualifier& qualifier, bool saveInTable );
	void AddMatchRight_S( CQualifier& qualifier, bool saveInTable );
	// matching duplicate of S-variables
	void AddMatchLeftDuplicate_S( TTableIndex, bool saveInTable );
	void AddMatchRightDuplicate_S( TTableIndex, bool saveInTable );
	// matching W-variables
	void AddMatchLeft_W( CQualifier& qualifier, bool saveInTable );
	void AddMatchRight_W( CQualifier& qualifier, bool saveInTable );
	// matching duplicate of WV-variables
	void AddMatchLeftDuplicate_WV( TTableIndex, bool saveInTable );
	void AddMatchRightDuplicate_WV( TTableIndex, bool saveInTable );
	// matching duplicate of E-variables
	void AddMatchLeftDuplicate_E( TTableIndex, bool saveInTable );
	void AddMatchRightDuplicate_E( TTableIndex, bool saveInTable );
	// matching closed V-variables
	void AddMatchClosed_V( CQualifier& qualifier, bool saveInTable );
	// matching closed E-variables
	void AddMatchClosed_E( CQualifier& qualifier, bool saveInTable );
	// matching V-variables by qualifier
	void AddMacthLeftMaxByQualifier_V( CQualifier& qualifier,
		bool saveInTable );
	void AddMacthRightMaxByQualifier_V( CQualifier& qualifier,
		bool saveInTable );
	// matching E-variables by qualifier
	void AddMacthLeftMaxByQualifier_E( CQualifier& qualifier,
		bool saveInTable );
	void AddMacthRightMaxByQualifier_E( CQualifier& qualifier,
		bool saveInTable );
	// matching V-variable
	void AddMatchLeft_V( CQualifier& qualifier, bool saveInTable );
	void AddMatchRight_V( CQualifier& qualifier, bool saveInTable );
	// matching E-variable
	void AddMatchLeft_E( CQualifier& qualifier, bool saveInTable );
	void AddMatchRight_E( CQualifier& qualifier, bool saveInTable );
	// making operations
	void AddInsertChar( TChar c );
	void AddInsertLabel( TLabel label );
	void AddInsertNumber( TNumber number );
	void AddInsertLeftParen();
	void AddInsertRightParen();
	void AddInsertRightBracket();
	void AddMove_S( TTableIndex );
	void AddCopy_S( TTableIndex );
	void AddMove_E( TTableIndex );
	void AddCopy_E( TTableIndex );
	void AddMove_WV( TTableIndex );
	void AddCopy_WV( TTableIndex );

	TStackIndex MaxStackDepth() const { return maxStackDepth; }

private:
	COperationList operations;
	COperationNode* savedOperation;
	TStackIndex stackDepth;
	TStackIndex maxStackDepth;
	TStackIndex maxRuleStackDepth;

	void addNoArgumensOperation( TOperationType type );
	void addCharOperation( TOperationType type, TChar c );
	void addLabelOperation( TOperationType type, TLabel label );
	void addNumberOperation( TOperationType type, TNumber number );
	void addTableIndexOperation( TOperationType type, TTableIndex tableIndex );
	void addQualifierIndexOperation( TOperationType type,
		CQualifier& qualifier );
	void addOperation_VE( TOperationType type );
	void addOperation_VE( TOperationType type, CQualifier& qualifier );
	void addStackDecrementOperation( TStackIndex size );

	TQualifierIndex registerQualifier( CQualifier& qualifier );
	COperation* addOperation( TOperationType type );
	void addOperation( const COperation& operation );

	void increaseStackDepth();
	void decreaseStackDepth( TStackIndex size );
};

//-----------------------------------------------------------------------------

} // end of namespace refal2
