#ifndef _LEXEM__H_
#define _LEXEM__H_

#include <string>

using std::string;

enum LexemType
{
	INDEFINITELY,
	COMMA,
	LESS,
	GREAT,
	LEFT_PAREN,
	RIGHT_PAREN,
	LINE_FEED,
	PARAGRAPH,
	EQUALITY,
	NUMBER,
	STRING,
	QUALIFIER,
	LABEL,
	IDENTIFIER
};

struct SimpleLexem
{
public:
	SimpleLexem(LexemType t = INDEFINITELY)
		: type(t)
	{
	}
	
	LexemType type;
};

struct NumberLexem : public SimpleLexem
{
public:
	NumberLexem(LexemType t = INDEFINITELY, unsigned int n = 0)
		: SimpleLexem(t), value(n)
	{
	}
	
	unsigned int value;
};

struct StringLexem : public SimpleLexem
{
public:
	StringLexem(LexemType t = INDEFINITELY)
		: SimpleLexem(t)
	{
	}

	string value;
};

class Lexem
{
private:
	Lexem(const Lexem&);
	Lexem &operator=(const Lexem &);
public:
	Lexem(SimpleLexem *lexem = 0)
		: l(lexem)
	{
	}
	~Lexem();

	Lexem(Lexem&);
	Lexem &operator=(Lexem&);
	Lexem &operator=(SimpleLexem*);
	SimpleLexem *get() const
	{
		return l;
	}
	SimpleLexem &operator*() const
	{
		return *l;
	}
	SimpleLexem *operator->() const
	{
		return l;
	}
	operator bool() const
	{
		return l;
	}
private:
	SimpleLexem *l;
	void freep();
};

#endif
