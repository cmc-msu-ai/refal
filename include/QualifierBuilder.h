#pragma once

#include <Refal2.h>

namespace Refal2 {

class CQualifierBuilder {
public:
	CQualifierBuilder()	{ Reset(); }
	
	void Reset();
	void Export( CQualifier& qualifier );
	
	bool IsNegative() const { return negative; }
	
	void AddChar( const TChar c );
	void AddLabel( const TLabel label );
	void AddNumber( const TNumber number );
	void AddQualifier( const CQualifier& qualifier );
	void AddNegative() { negative = !negative; }
	
	void AddS() { AddF(); AddN(); AddO(); }
	void AddF();
	void AddN();
	void AddO();
	void AddL();
	void AddD();
	void AddW() { AddS(); AddB(); }
	void AddB();
	
	static CAnsiSet MakeFromString( const char* ansiString );
	
	static const char* Alphabet;
	static const char* Numbers;
	static const CAnsiSet AnsiL;
	static const CAnsiSet AnsiD;
	
private:
	CQualifierBuilder( const CQualifierBuilder& );
	CQualifierBuilder& operator=( const CQualifierBuilder& );
	
	bool negative;
	
	enum TStatus {
		S_None,
		S_Yes,
		S_No
	};
	TStatus terms;
	
	CSetBuilder<TChar> charsBuilder;
	CSetBuilder<TLabel> labelsBuilder;
	CSetBuilder<TNumber> numbersBuilder;
	
	CAnsiSet ansichars;
	CAnsiSet ansicharsFixed;
};

} // end of namespace refal2
