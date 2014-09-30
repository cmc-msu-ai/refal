#pragma once

#include "LinkBuilder.h"
#include "Common.h"

namespace Refal2 {

class CFunctionBuilder : private CLinkBuilder {
public:
	CFunctionBuilder(): begin(0) { Reset(); }
	~CFunctionBuilder() { Reset(); }

	static void Free(CRule** rule);

	void Reset();
	void Get(CRule** rule);

	void RightDirection();
	void EndOfLeftPart();
	void EndOfRule();
	
	using CLinkBuilder::Char;
	using CLinkBuilder::Label;
	using CLinkBuilder::Number;
	using CLinkBuilder::Variable;
	using CLinkBuilder::LeftParen;
	using CLinkBuilder::RightParen;
	using CLinkBuilder::LeftBracket;
	using CLinkBuilder::RightBracket;
private:
	CFunctionBuilder(const CFunctionBuilder&);
	CFunctionBuilder& operator=(const CFunctionBuilder&);
	
	void rule();

	bool current_rule_was_created;
	CRule* begin;
	CRule* end;
};

} // end of namespace Refal2

