#ifndef _LEXEM__H_
#define _LEXEM__H_

#include <string>

using std::string;

enum LexemType
{
	INDEFINITELY,
	LETTER_OR_DIGIT,
	LEFT_PAREN,
	RIGHT_PAREN,
	COMMA,
	EQUALITY,
	LESS,
	GREAT,
	QUALIFIER,
	LABEL,
	NUMBER,
	STRING,
	LINE_FEED,
	SPACE
};

struct SimpleLexem
{
public:
	SimpleLexem(unsigned int l = 0, unsigned int p = 0,
				LexemType t = INDEFINITELY)
		: type(t), line(l), offset(p)
	{
	}
	
	LexemType type;
	unsigned int line, offset;
};

struct NumberLexem : public SimpleLexem
{
public:
	NumberLexem(unsigned int l = 0, unsigned int p = 0, unsigned int n = 0)
		: SimpleLexem(l, p, NUMBER), value(n)
	{
	}
	
	unsigned int value;
};

struct LetterOrDigitLexem : public SimpleLexem
{
public:
	LetterOrDigitLexem(unsigned int l = 0, unsigned int p = 0, char v = '\0')
		: SimpleLexem(l, p, LETTER_OR_DIGIT), value(v)
	{
	}

	char value;
};

struct StringLexem : public SimpleLexem
{
public:
	StringLexem(unsigned int l = 0, unsigned int p = 0,
					LexemType t = INDEFINITELY)
		: SimpleLexem(l, p, t)
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
