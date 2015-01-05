#pragma once

#include <Refal2.h>

namespace Refal2 {

class COperationsBuilder {
public:
	COperationsBuilder();

	COperation* Reserve();
	COperationOperation* ReserveOperation();
	COperationInt* ReserveInt();
	COperationIntInt* ReserveIntInt();
	COperationUnit* ReserveUnit();
	COperationQualifier* ReserveQualifier();

	COperation* Add(COperation::TOperationType type);
	COperationOperation* Add(COperation::TOperationType type,
		COperation* operation);
	COperationInt* Add(COperation::TOperationType type, int x);
	COperationIntInt* Add(COperation::TOperationType type, int x, int y);
	COperationUnit* Add(COperation::TOperationType type,
		const CUnit& value);
	COperationQualifier* Add(COperation::TOperationType type,
		const CQualifier* qualifier);

private:
	COperation* last_op;
	
};

} // end of namespace refal2
