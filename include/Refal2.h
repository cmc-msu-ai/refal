#pragma once

#include <Common.h>

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
class CFunctionBuilder;

class CLabelTable;

class COperation;
class COperationsBuilder;
class COperationsExecuter;

class CParser;

class CQualifier;
class CQualifierBuilder;

class CScanner;

class CUnit;
class CUnitList;

class CVariable;
class CVariables;
class CVariablesBuilder;

// declaration of global variables
extern CLabelTable LabelTable;

// tools functions
void MakeLower( std::string& data );

} // end of namespace Refal2

#include <Dictionary.h>
#include <Errors.h>
#include <NodeList.h>
#include <FastSet.h>
#include <SetBuilder.h>
#include <Dictionary.h>
#include <Unit.h>
#include <Qualifier.h>
#include <QualifierBuilder.h>
#include <OperationsBuilder.h>
#include <Variables.h>
#include <Token.h>
#include <Function.h>
#include <FunctionCompiler.h>
#include <Label.h>
#include <ModuleBuilder.h>
#include <QualifierParser.h>
#include <RuleParser.h>
#include <DirectiveParser.h>
#include <Parser.h>
#include <Scanner.h>
#include <OperationsExecuter.h>
