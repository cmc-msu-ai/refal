#pragma once

namespace Refal2 {

// typedefs
typedef char TChar;
typedef int TNumber;
typedef int TLabel;
typedef unsigned char TVariableName;
typedef char TVariableType;
typedef int TVariableIndex;
typedef int TTableIndex;

// classes
class CExecuter;

class CFunction;
struct CFunctionRule;
class CFunctionCompiler;
class IFunctionBuilderListener;
class CFunctionBuilder;

class CLabelTable;

struct COperationOperation;
struct COperationInt;
struct COperationIntInt;
struct COperationUnit;
struct COperationQualifier;

class COperations;

class COperationsBuilder;

class IParserListener;
class CParser;

class CQualifier;
class CQualifierBuilder;

class IScannerListener;
class CScanner;

class CUnit;
class CUnitNode;
class CUnitList;

class CVariable;
class CVariables;
class CVariablesBuilder;

// declaration of global variables
extern CLabelTable LabelTable;

} // end of namespace Refal2

#include <Assert.h>
#include <Errors.h>
#include <Listener.h>
#include <FastSet.h>
#include <SetBuilder.h>
#include <Unit.h>
#include <Qualifier.h>
#include <QualifierBuilder.h>
#include <Variables.h>
#include <Function.h>
#include <FunctionCompiler.h>
#include <Label.h>
#include <Scanner.h>
#include <Parser.h>
#include <Operation.h>
#include <Operations.h>
#include <OperationsBuilder.h>
#include <Executer.h>
