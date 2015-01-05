#include <Refal2.h>

namespace Refal2 {

COperationsBuilder::COperationsBuilder()
{
	last_op = new COperation(COperation::OT_goto);
}



COperation* COperationsBuilder::Reserve()
{
	COperation* op =
		static_cast<COperation*>(::operator new(sizeof(COperation)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperationOperation* COperationsBuilder::ReserveOperation()
{
	COperationOperation* op = static_cast<COperationOperation*>(
		::operator new(sizeof(COperationOperation)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperationInt* COperationsBuilder::ReserveInt()
{
	COperationInt* op =
		static_cast<COperationInt*>(::operator new(sizeof(COperationInt)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperationIntInt* COperationsBuilder::ReserveIntInt()
{
	COperationIntInt* op = static_cast<COperationIntInt*>(
		::operator new(sizeof(COperationIntInt)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperationUnit* COperationsBuilder::ReserveUnit()
{
	COperationUnit* op = static_cast<COperationUnit*>(
		::operator new(sizeof(COperationUnit)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperationQualifier* COperationsBuilder::ReserveQualifier()
{
	COperationQualifier* op = static_cast<COperationQualifier*>(
		::operator new(sizeof(COperationQualifier)));
	last_op->next = op;
	last_op = op;
	return op;
}

COperation* COperationsBuilder::Add(COperation::TOperationType type)
{
	COperation* op = Reserve();
	op->type = type;

	return op;
}

COperationOperation* COperationsBuilder::Add(COperation::TOperationType type,
	COperation* operation)
{
	COperationOperation* op = ReserveOperation();
	op->type = type;
	op->operation = operation;

	return op;
}

COperationInt* COperationsBuilder::Add(COperation::TOperationType type, int x)
{
	COperationInt* op = ReserveInt();
	op->type = type;
	op->x = x;

	return op;
}

COperationIntInt* COperationsBuilder::Add(COperation::TOperationType type,
	int x, int y)
{
	COperationIntInt* op = ReserveIntInt();
	op->type = type;
	op->x = x;
	op->y = y;

	return op;
}

COperationUnit* COperationsBuilder::Add(COperation::TOperationType type,
	const CUnit& value)
{
	COperationUnit* op = ReserveUnit();
	op->type = type;
	op->value = value;

	return op;
}

COperationQualifier* COperationsBuilder::Add(COperation::TOperationType type,
	const CQualifier* qualifier)
{
	COperationQualifier* op = ReserveQualifier();
	op->type = type;
	op->qualifier = qualifier;

	return op;
}

} // end of namespace refal2
