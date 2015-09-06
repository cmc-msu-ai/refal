#pragma once

#include <Common.h>

#ifdef _DEBUG
#define DEBUG_PRINT( arg ) std::cout << "DEBUG " << arg << std::endl;
#else
#define DEBUG_PRINT( arg )
#endif

namespace Refal2 {

// typedefs
typedef char TChar;
typedef int TNumber;
typedef int TLabel;
typedef unsigned char TVariableName;
typedef char TVariableTypeTag;
typedef int TVariableIndex;
typedef int TTableIndex;
typedef int TRuntimeModuleId;

// classes
class CExecuter;

class CRule;
class CPreparatoryFunction;
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

// tools functions
void MakeLower( std::string& data );
bool CompareNoCase( char a, char b );
bool CompareNoCase( const std::string& str1, const std::string& str2 );

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
#include <FunctionCompiler.h>
#include <Function.h>
#include <Receptacle.h>
#include <EmbeddedFunctions.h>
#include <Program.h>
#include <ModuleBuilder.h>
#include <QualifierParser.h>
#include <RuleParser.h>
#include <DirectiveParser.h>
#include <Parser.h>
#include <Scanner.h>
#include <OperationsExecuter.h>
