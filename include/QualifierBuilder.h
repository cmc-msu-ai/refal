#pragma once

#include "SetBuilder.h"
#include "Qualifier.h"
#include <bitset>

namespace Refal2 {

class CQualifierBuilder {
public:
	CQualifierBuilder()
	{
		Reset();
	}

	void Reset();
	void Get(CQualifier* qualifier);
	
	void Label(TLabel label);
	void Number(TNumber number);
	void Char(TChar c);
	void Qualifier(const CQualifier& qualifier);
	void Negative();
	bool IsNegative() const
	{
		return negative;
	}
	void S()
	{
		F();
		N();
		O();
	}
	void F();
	void N();
	void O();
	void L();
	void D();
	void W()
	{
		S();
		B();
	}
	void B();

private:
	CQualifierBuilder(const CQualifierBuilder&);
	CQualifierBuilder& operator=(const CQualifierBuilder&);

	enum TStatus {
		S_none,
		S_yes,
		S_no
	};

	bool negative;
	
	TStatus terms;
	TStatus chars;

	CSetBuilder<TChar> chars_builder;
	CSetBuilder<TLabel> labels_builder;
	CSetBuilder<TNumber> numbers_builder;

	TAnsiSet ansichars;
	TAnsiSet ansichars_fixed;

	static void make_from_string(TAnsiSet& set, const char* ansi);
	static TAnsiSet make_ansi_L();
	static TAnsiSet make_ansi_D();

	static const TAnsiSet ansi_L;
	static const TAnsiSet ansi_D;
};

} // end of namespace refal2

