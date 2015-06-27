#pragma once

#include <Refal2.h>

namespace Refal2 {

const TTableIndex InvalideTableIndex = -1;

class CHole : public CUnitList {
public:
	CHole( CUnitList* hole, TTableIndex left, TTableIndex right );
	CHole( CUnitNode* first, CUnitNode* last,
		TTableIndex left, TTableIndex right );
	
	CHole( const CHole& );
	CHole& operator=( const CHole& );
	
	void SetLeft( TTableIndex _left ) { left = _left; }
	TTableIndex GetLeft() const { return left; }
	void SetRight( TTableIndex _right ) { right = _right; }
	TTableIndex GetRight() const { return right; }
	
private:
	TTableIndex left;
	TTableIndex right;
};

typedef CNodeList<CHole> CHoleList;
typedef CHoleList::CNodeType CHoleNode;

const int MaxCountOfDifferentVariablesInRule = 64;

typedef std::bitset<MaxCountOfDifferentVariablesInRule> CVariablesMask;

struct CHolesTuple {
	CHoleList holes;
	int stackDepth;
	
	explicit CHolesTuple( int _stackDepth ): stackDepth( _stackDepth ) {}
};

class CFunctionCompilerBase : public COperationsBuilder {
protected:
	CVariables variables;
};

class CLeftPartCompiler : public CFunctionCompilerBase {
protected:
	CLeftPartCompiler();
	
	void CompileLeftPart( CUnitList* leftPart, bool isRightDirection );
	
private:
	void removeHole();
	
	typedef void (COperationsBuilder::*TMatchDuplicateFunction)
		( TVariableIndex variableIndex, bool saveInTable );
	typedef void (COperationsBuilder::*TMatchFunction)
		( CQualifier&, bool addValueToTable );
	
	bool setVariable( TVariableIndex variableIndex);
	void matchVariable( TVariableIndex variableIndex, TMatchFunction function);
	void matchDuplicateVariable( TVariableIndex variableIndex,
		TMatchDuplicateFunction function);
	
	//inline bool isMarkedVariable(CUnitNode* unit);
	bool isVE( CUnitNode* unit ) const;
	bool isFreeVE( CUnitNode* unit ) const;
	
	//CVariablesMask makeVariablesMask(const CHole& hole) const;
	//void splitIntoClasses(CHole* const holes);
	
	void matchVE( bool isRightDirection );
	void checkBorders();
	void matchElement();
	bool tryMatchLeftVariable( CUnitNode* left );
	bool tryMatchRightVariable( CUnitNode* right );
	
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
	CHoleNode* hole;
	
#if 0
	CVariablesMask markedVariables;
	int markedStackDepth[MaxCountOfDifferentVariablesInRule];
	std::vector<CHolesTuple> classes;
	int stackDepth;
#endif
};

#if 0
inline bool CLeftPartCompiler::isMarkedVariable(CUnitNode* unit)
{
	return ( unit != 0 && unit->IsVariable() &&
		markedVariables.test( unit->Variable() ) );
}
#endif

inline bool CLeftPartCompiler::isVE( CUnitNode* unit ) const
{
	return ( unit != 0 && unit->IsVariable() &&
		variables.GetVariable( unit->Variable() ).TypeIs( 'v', 'e' ) );
}

inline bool CLeftPartCompiler::isFreeVE( CUnitNode* unit ) const
{
	return ( isVE(unit) && !variables.IsSet( unit->Variable() ) );
}

// ----------------------------------------------------------------------------

class CRightPartCompiler : public CLeftPartCompiler {
protected:
	void CompileRightPart( CUnitList* rightPart );
};

class CFunctionCompiler : public CRightPartCompiler {
public:
	void Compile( CFunction* function );

private:
	void compileRule( CFunctionRule* rule );
};

} // end of namespace refal2
