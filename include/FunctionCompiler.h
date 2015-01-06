#pragma once

#include <bitset>
#include <vector>
#include <Refal2.h>

namespace Refal2 {

const TTableIndex InvalideTableIndex = -1;

struct CHole {
	CUnitList hole;
	TTableIndex left, right;
	CHole* nextHole;
	
	CHole(TTableIndex _left = 0, TTableIndex _right = 1):
		left(_left), right(_right), nextHole(0) {}
};

const int MaxCountOfDifferentVariablesInRule = 64;

typedef std::bitset<MaxCountOfDifferentVariablesInRule> TVariablesMask;

struct CHolesTuple {
	CHole* firstHole;
	int stackDepth;
	
	explicit CHolesTuple(int _stackDepth, CHole* _firstHole = 0):
		firstHole(_firstHole), stackDepth(_stackDepth) {}
};

class CFunctionCompiler {
public:
	void compileRule(CFunctionRule* rule);

private:
	void removeCurrentHole();
	inline CUnitNode* detachLeftUnitInCurrentHole();
	inline CUnitNode* detachRightUnitInCurrentHole();
	
	inline bool isMarkedVariable(CUnitNode* unit);
	inline bool isVE(CUnitNode* unit) const;
	inline bool isFreeVE(CUnitNode* unit) const;

	TVariablesMask makeVariablesMask(const CHole& hole) const;
	void splitIntoClasses(CHole* const holes);
	
	void matchVE();
	
	void matchElement();
	bool tryMatchLeftVariable(CUnitNode* left);
	bool tryMatchRightVariable(CUnitNode* right);
	
	void matchEmptyExpression();
	void matchClosedE();
	void matchLeftParens();
	void matchRightParens();
	void matchLeftSymbol();
	void matchRightSymbol();
	void matchLeftS();
	void matchRightS();
	void matchLeftW();
	void matchRightW();
	void matchLeftDuplicateVE();
	void matchRightDuplicateVE();

	TTableIndex currentLeft;
	TTableIndex currentRight;
	TTableIndex tableIndex;
	TVariablesMask markedVariables;
	int markedStackDepth[MaxCountOfDifferentVariablesInRule];
	std::vector<CHolesTuple> classes;
	CVariables variables;
	CHole* currentHole;
	CHole* firstHole;
	int stackDepth;
};

inline CUnitNode* CFunctionCompiler::detachLeftUnitInCurrentHole()
{
	CUnitNode* tmp = currentHole->hole.GetFirst();
	currentHole->hole.Detach( tmp );
	return tmp;
}

inline CUnitNode* CFunctionCompiler::detachRightUnitInCurrentHole()
{
	CUnitNode* tmp = currentHole->hole.GetLast();
	currentHole->hole.Detach( tmp );
	return tmp;
}

inline bool CFunctionCompiler::isMarkedVariable(CUnitNode* unit)
{
	return ( unit != 0 && unit->IsVariable() &&
		markedVariables.test( unit->Variable() ) );
}

inline bool CFunctionCompiler::isVE(CUnitNode* unit) const
{
	return ( unit != 0 && unit->IsVariable() &&
		variables.GetVariable( unit->Variable() ).TypeIs( 'v', 'e' ) );
}

inline bool CFunctionCompiler::isFreeVE(CUnitNode* unit) const
{
	return ( isVE(unit) && !variables.IsSet( unit->Variable() ) );
}

} // end of namespace refal2
