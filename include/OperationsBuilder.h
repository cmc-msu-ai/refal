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
	/* matching operation */
	OT_MatchEmptyExpression,
	OT_MatchLeftChar, /* TChar */
	OT_MatchLeftLabel, /* TLabel */
	OT_MatchLeftNumber, /* TNumber */
	OT_MatchRightChar, /* TChar */
	OT_MatchRightLabel, /* TLabel */
	OT_MatchRightNumber, /* TNumber */
	OT_MatchLeftParens,
	OT_MatchRightParens,
	OT_SetLeftBorder, /* TTableIndex */
	OT_SetRightBorder, /* TTableIndex */
	OT_MatchLeft_S,
	OT_MatchRight_S,
	OT_MatchLeftDuplicate_S, /* TTableIndex */
	OT_MatchRightDuplicate_S, /* TTableIndex */
	OT_MatchLeft_W,
	OT_MatchRight_W,
	OT_MatchClosed_E,
	OT_MatchLeftDuplicate_WVE, /* TTableIndex */
	OT_MatchRightDuplicate_WVE, /* TTableIndex */
	OT_DecrementStackDepth, /* TUint32 */
	OT_CheckNotEmpty,
	OT_CheckQualifier_S, /* TQualifierIndex */
	OT_CheckQualifier_WVE, /* TQualifierIndex */
	OT_MacthLeftMaxByQualifier, /* TQualifierIndex */
	OT_MacthRightMaxByQualifier, /* TQualifierIndex */
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
	/* result builder operation */
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
	friend class COperationBuilder;

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
	/* matching operation */
	void AddMatchEmptyExpression();
	void AddMatchLeftChar(const TChar c);
	void AddMatchLeftLabel(const TLabel label);
	void AddMatchLeftNumber(const TNumber number);
	void AddMatchRightChar(const TChar c);
	void AddMatchRightLabel(const TLabel label);
	void AddMatchRightNumber(const TNumber number);
	void AddMatchLeftParens();
	void AddMatchRightParens();
	void AddSetLeftBorder(const TTableIndex);
	void AddSetRightBorder(const TTableIndex);
	void AddMatchLeft_S(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_S(CQualifier* qualifier, const bool saveInTable);
	void AddMatchLeftDuplicate_S(const TTableIndex, const bool saveInTable);
	void AddMatchRightDuplicate_S(const TTableIndex, const bool saveInTable);
	void AddMatchLeft_W(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_W(CQualifier* qualifier, const bool saveInTable);
	void AddMatchClosed_E(CQualifier* qualifier, const bool saveInTable);
	void AddMatchLeftDuplicate_WVE(const TTableIndex, const bool saveInTable);
	void AddMatchRightDuplicate_WVE(const TTableIndex, const bool saveInTable);
	void AddDecrementStackDepth(const int count);
	void AddCheckNotEmpty();
	/* match left VE-variable */
	void AddMatchLeft_E(CQualifier* qualifier, const bool saveInTable);
	void AddMatchLeft_V(CQualifier* qualifier, const bool saveInTable);
	/* match right VE-variable */
	void AddMatchRight_E(CQualifier* qualifier, const bool saveInTable);
	void AddMatchRight_V(CQualifier* qualifier, const bool saveInTable);
	/* match by qualifier */
	void AddMacthLeftMaxByQualifier(CQualifier* qualifier,
		const bool saveInTable);
	void AddMacthRightMaxByQualifier(CQualifier* qualifier,
		const bool saveInTable);
	/* result builder operation */
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
