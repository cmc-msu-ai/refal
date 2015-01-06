#pragma once

#include <bitset>
#include <Refal2.h>

namespace Refal2 {

class CQualifierBuilder {
public:
	CQualifierBuilder()	{ Reset(); }

	void Reset();
	void Export(CQualifier* qualifier);
	
	bool IsNegative() const { return negative; }

	void AddChar(TChar c);
	void AddLabel(TLabel label);
	void AddNumber(TNumber number);
	void AddQualifier(const CQualifier& qualifier);
	void AddNegative() { negative = !negative; }

	void AddS() { AddF(); AddN(); AddO(); }
	void AddF();
	void AddN();
	void AddO();
	void AddL();
	void AddD();
	void AddW() { AddS(); AddB(); }
	void AddB();

	static TAnsiSet MakeFromString(const char* ansiString);

	static const char* Alphabet;
	static const char* Numbers;
	static const TAnsiSet AnsiL;
	static const TAnsiSet AnsiD;

private:
	CQualifierBuilder(const CQualifierBuilder&);
	CQualifierBuilder& operator=(const CQualifierBuilder&);

	bool negative;

	enum TStatus {
		S_none,
		S_yes,
		S_no
	};
	
	TStatus terms;
	TStatus chars;

	CSetBuilder<TChar> charsBuilder;
	CSetBuilder<TLabel> labelsBuilder;
	CSetBuilder<TNumber> numbersBuilder;

	TAnsiSet ansichars;
	TAnsiSet ansicharsFixed;
};

} // end of namespace refal2
