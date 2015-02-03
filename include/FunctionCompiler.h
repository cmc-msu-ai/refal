#pragma once

#include <bitset>
#include <vector>
#include <Refal2.h>

namespace Refal2 {

const TTableIndex InvalideTableIndex = -1;

class CHole : public CUnitList {
public:
	CHole(CUnitList* hole, const TTableIndex left, const TTableIndex right);
	CHole(TUnitNode* const first, TUnitNode* const last,
		const TTableIndex left, const TTableIndex right);
	
	CHole(const CHole&);
	CHole& operator=(const CHole&);
	
	void SetLeft(const TTableIndex _left) { left = _left; }
	TTableIndex GetLeft() const { return left; }
	void SetRight(const TTableIndex _right) { right = _right; }
	TTableIndex GetRight() const { return right; }
	
private:
	TTableIndex left;
	TTableIndex right;
};

typedef CNodeList<CHole> CHoleList;

const int MaxCountOfDifferentVariablesInRule = 64;

typedef std::bitset<MaxCountOfDifferentVariablesInRule> TVariablesMask;

struct CHolesTuple {
	CHoleList holes;
	int stackDepth;
	
	explicit CHolesTuple(int _stackDepth): stackDepth(_stackDepth) {}
};

class CFunctionCompilerBase : protected COperationsBuilder {
protected:
	CVariables variables;
};

class CLeftPartCompiler : public CFunctionCompilerBase {
protected:
	CLeftPartCompiler();
	
	void CompileLeftPart(CUnitList* leftPart, const bool isRightDirection);
	
private:
	void removeHole();
	
	typedef void (COperationsBuilder::*TMatchDuplicateFunction)
		(const TVariableIndex variableIndex, const bool saveInTable);
	typedef void (COperationsBuilder::*TMatchFunction)
		(CQualifier*, const bool addValueToTable);
	
	bool setVariable(const TVariableIndex variableIndex);
	void matchVariable(const TVariableIndex variableIndex,
		const TMatchFunction function);
	void matchDuplicateVariable(const TVariableIndex variableIndex,
		const TMatchDuplicateFunction function);
	
	//inline bool isMarkedVariable(TUnitNode* unit);
	inline bool isVE(TUnitNode* unit) const;
	inline bool isFreeVE(TUnitNode* unit) const;
	
	//TVariablesMask makeVariablesMask(const CHole& hole) const;
	//void splitIntoClasses(CHole* const holes);
	
	void matchVE(const bool isRightDirection);
	
	void matchElement();
	bool tryMatchLeftVariable(TUnitNode* left);
	bool tryMatchRightVariable(TUnitNode* right);
	
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
	
	TTableIndex top;
	TTableIndex left;
	TTableIndex right;
	
	CHoleList holes;
	CHoleList::TNode* hole;
	
#if 0
	TVariablesMask markedVariables;
	int markedStackDepth[MaxCountOfDifferentVariablesInRule];
	std::vector<CHolesTuple> classes;
	int stackDepth;
#endif
};

#if 0
inline bool CLeftPartCompiler::isMarkedVariable(TUnitNode* unit)
{
	return ( unit != 0 && unit->IsVariable() &&
		markedVariables.test( unit->Variable() ) );
}
#endif

inline bool CLeftPartCompiler::isVE(TUnitNode* unit) const
{
	return ( unit != 0 && unit->IsVariable() &&
		variables.GetVariable( unit->Variable() )->TypeIs( 'v', 'e' ) );
}

inline bool CLeftPartCompiler::isFreeVE(TUnitNode* unit) const
{
	return ( isVE(unit) && !variables.IsSet( unit->Variable() ) );
}

// ----------------------------------------------------------------------------

class CRightPartCompiler : public CLeftPartCompiler {
protected:
	void CompileRightPart(CUnitList* rightPart);
};

class CFunctionCompiler : public CRightPartCompiler {
public:
	void Compile(CFunction* function);

private:
	void compileRule(CFunctionRule* rule);
};

} // end of namespace refal2
