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

	inline bool CompareLeftAndRight(const TTableIndex leftToCompare,
		const TTableIndex rightToCompare) const;

	void SetLeft(const TTableIndex _left) { left = _left; }
	TTableIndex GetLeft() const { return left; }
	void SetRight(const TTableIndex _right) { right = _right; }
	TTableIndex GetRight() const { return right; }

private:
	TTableIndex left;
	TTableIndex right;
};

inline bool CHole::CompareLeftAndRight(const TTableIndex leftToCompare,
	const TTableIndex rightToCompare) const
{
	return ( left == leftToCompare && right == rightToCompare );
}

typedef CNodeList<CHole> THoleList;

const int MaxCountOfDifferentVariablesInRule = 64;

typedef std::bitset<MaxCountOfDifferentVariablesInRule> TVariablesMask;

struct CHolesTuple {
	THoleList holes;
	int stackDepth;
	
	explicit CHolesTuple(int _stackDepth): stackDepth(_stackDepth) {}
};

class CFunctionCompilerBase {
protected:
	CVariables variables;
};

class CLeftPartCompiler : public CFunctionCompilerBase {
protected:
	CLeftPartCompiler();

	void CompileLeftPart(CUnitList* leftPart, const bool isRightDirection);

private:
	void removeHole();

	inline bool isMarkedVariable(TUnitNode* unit);
	inline bool isVE(TUnitNode* unit) const;
	inline bool isFreeVE(TUnitNode* unit) const;

	TVariablesMask makeVariablesMask(const CHole& hole) const;
	void splitIntoClasses(CHole* const holes);
	
	void matchVE();
	
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

	THoleList holes;
	THoleList::TNode* hole;

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
