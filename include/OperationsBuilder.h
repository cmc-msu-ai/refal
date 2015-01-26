#pragma once

#include <Refal2.h>

namespace Refal2 {

typedef int TUint32;
typedef int TOperationAddress;
typedef int TQualifierIndex;

enum TOperationType {
	OT_Goto, /* TOperationAddress */
	OT_InsertJump,
	OT_MatchingComplete,
	OT_Return,
	OT_SetLeftBorder, /* TTableIndex */
	OT_SetRightBorder, /* TTableIndex */
	OT_DecrementStackDepth, /* TUint32 */
	OT_CheckNotEmpty,
	/* matching empty expression */
	OT_MatchEmptyExpression,
	/* matching symbols */
	OT_MatchLeftChar, /* TChar */
	OT_MatchLeftLabel, /* TLabel */
	OT_MatchLeftNumber, /* TNumber */
	OT_MatchRightChar, /* TChar */
	OT_MatchRightLabel, /* TLabel */
	OT_MatchRightNumber, /* TNumber */
	/* matching parens */
	OT_MatchLeftParens,
	OT_MatchRightParens,
	/* matching S-variables */
	OT_MatchLeft_S,
	OT_MatchLeftSaveToTable_S,
	OT_MatchLeftWithQualifier_S, /* TQualifierIndex */
	OT_MatchLeftWithQualifierSaveToTable_S, /* TQualifierIndex */
	OT_MatchRight_S,
	OT_MatchRightSaveToTable_S,
	OT_MatchRightWithQualifier_S, /* TQualifierIndex */
	OT_MatchRightWithQualifierSaveToTable_S, /* TQualifierIndex */
	/* matching duplicate of S-variables */
	OT_MatchLeftDuplicate_S, /* TTableIndex */
	OT_MatchLeftDuplicateSaveToTable_S, /* TTableIndex */
	OT_MatchRightDuplicate_S, /* TTableIndex */
	OT_MatchRightDuplicateSaveToTable_S, /* TTableIndex */
	/* matching W-variables */
	OT_MatchLeft_W,
	OT_MatchLeftSaveToTable_W,
	OT_MatchLeftWithQualifier_W, /* TQualifierIndex */
	OT_MatchLeftWithQualifierSaveToTable_W, /* TQualifierIndex */
	OT_MatchRight_W,
	OT_MatchRightSaveToTable_W,
	OT_MatchRightWithQualifier_W, /* TQualifierIndex */
	OT_MatchRightWithQualifierSaveToTable_W, /* TQualifierIndex */
	/* matching duplicate of W-variables */
	OT_MatchLeftDuplicate_WVE, /* TTableIndex */
	OT_MatchLeftDuplicateSaveToTable_WVE, /* TTableIndex */
	OT_MatchRightDuplicate_WVE, /* TTableIndex */
	OT_MatchRightDuplicateSaveToTable_WVE, /* TTableIndex */
	/* matching closed E-variables */
	OT_MatchClosed_E,
	OT_MatchClosedSaveToTable_E,
	OT_MatchClosedWithQualifier_E, /* TQualifierIndex */
	OT_MatchClosedWithQualifierSaveToTable_E, /* TQualifierIndex */
	/* matching by qualifier */
	OT_MacthLeftMaxByQualifier, /* TQualifierIndex */
	OT_MacthLeftMaxByQualifierSaveToTable, /* TQualifierIndex */
	OT_MacthRightMaxByQualifier, /* TQualifierIndex */
	OT_MacthRightMaxByQualifierSaveToTable, /* TQualifierIndex */
	/* matching V-variable */
	/* matching E-variable */
#if 0
	/* match left VE-variable */
	OT_MatchLeftBegin_E,
	OT_MatchLeftBegin_V,
	OT_MatchLeftWithQulifierBegin_V, /* TQualifierIndex */
	OT_MatchLeft_E,
	OT_MatchLeftWithQulifier_E, /* TQualifierIndex */
	/* match right VE-variable */
	OT_MatchRightBegin_E,
	OT_MatchRightBegin_V,
	OT_MatchRightWithQulifierBegin_V, /* TQualifierIndex */
	OT_MatchRight_E,
	OT_MatchRightWithQulifier_E, /* TQualifierIndex */
#endif
	/* making operations */
	OT_InsertChar, /* TChar */
	OT_InsertLabel, /* TLabel */
	OT_InsertNumber, /* TNumber */
	OT_InsertLeftParen,
	OT_InsertRightParen,
	OT_InsertRightBracket,
	OT_Move_S, /* TTableIndex */
	OT_Copy_S, /* TTableIndex */
	OT_Move_E, /* TTableIndex */
	OT_Copy_E, /* TTableIndex */
	OT_Move_WV, /* TTableIndex */
	OT_Copy_WV /* TTableIndex */
};

/*
EX    = OT_MatchLeftBegin_E; OT_MatchLeftBegin_V; OT_MatchLeft_E;
E:Q:X = OT_MatchLeftBegin_E; OT_MatchLeftWithQulifierBegin_V; OT_MatchLeftWithQulifier_E;
VX    = OT_MatchLeftBegin_V; OT_MatchLeft_E;
V:Q:X = OT_MatchLeftWithQulifierBegin_V; OT_MatchLeftWithQulifier_E;
*/

class COperation {
	friend class COperationsBuilder;
	friend class COperationsExecuter;

private:
	COperation(const TOperationType _type): type( _type ) {}

	TOperationType type;
	union {
		TChar c;
		TLabel label;
		TNumber number;
		TUint32 stackDecrement;
		TTableIndex tableIndex;
		TQualifierIndex qualifier;
		TOperationAddress operation;
	};
};

class COperationsBuilder {
public:
	COperationsBuilder() {}
	void Reset() {}
	
	void AddMatchingComplete();
	void AddReturn();
	void AddDecrementStackDepth(const int count);
	void AddCheckNotEmpty();
	void AddSetLeftBorder(const TTableIndex);
	void AddSetRightBorder(const TTableIndex);
	/* matching empty expression */
	void AddMatchEmptyExpression();
	/* matching symbols */
	void AddMatchLeftChar(const TChar c);
	void AddMatchLeftLabel(const TLabel label);
	void AddMatchLeftNumber(const TNumber number);
	void AddMatchRightChar(const TChar c);
	void AddMatchRightLabel(const TLabel label);
	void AddMatchRightNumber(const TNumber number);
	/* matching parens */
	void AddMatchLeftParens();
	void AddMatchRightParens();
	/* matching S-variables */
	void AddMatchLeft_S(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_S(CQualifier* qualifier, const bool saveInTable);
	/* matching duplicate of S-variables */
	void AddMatchLeftDuplicate_S(const TTableIndex, const bool saveInTable);
	void AddMatchRightDuplicate_S(const TTableIndex, const bool saveInTable);
	/* matching W-variables */
	void AddMatchLeft_W(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_W(CQualifier* qualifier, const bool saveInTable);
	/* matching duplicate of W-variables */
	void AddMatchLeftDuplicate_WVE(const TTableIndex, const bool saveInTable);
	void AddMatchRightDuplicate_WVE(const TTableIndex, const bool saveInTable);
	/* matching closed E-variables */
	void AddMatchClosed_E(CQualifier* qualifier, const bool saveInTable);
	/* matching by qualifier */
	void AddMacthLeftMaxByQualifier(CQualifier* qualifier,
		const bool saveInTable);
	void AddMacthRightMaxByQualifier(CQualifier* qualifier,
		const bool saveInTable);
	/* matching V-variable */
	void AddMatchLeft_V(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_V(CQualifier* qualifier, const bool saveInTable);
	/* matching E-variable */
	void AddMatchLeft_E(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_E(CQualifier* qualifier, const bool saveInTable);
	/* making operations */
	void AddInsertChar(const TChar c);
	void AddInsertLabel(const TLabel label);
	void AddInsertNumber(const TNumber number);
	void AddInsertLeftParen();
	void AddInsertRightParen();
	void AddInsertRightBracket();
	void AddMove_S(const TTableIndex);
	void AddCopy_S(const TTableIndex);
	void AddMove_E(const TTableIndex);
	void AddCopy_E(const TTableIndex);
	void AddMove_WV(const TTableIndex);
	void AddCopy_WV(const TTableIndex);
	
private:
	COperation* addOperation(const TOperationType type);
};

} // end of namespace refal2
