#ifndef _SCANNER__H__
#define _SCANNER__H__

#include <iostream>
#include <string>

using std::istream;
using std::ostream;
using std::string;

class Lexem
{
	friend class Scanner;
public:
	Lexem();
	~Lexem();
	
	Lexem(const Lexem &l);
	Lexem &operator=(const Lexem &l);
public:
	enum LexemType
	{
		INDEFINITELY, COMMA, LESS, GREAT, NUMBER, STRING, QUALIFIER, LABEL,
		PARAGRAPH, LINE_FEED, LEFT_PAREN, IDENTIFIER, EQUALITY, RIGHT_PAREN
	};
private:
	LexemType type;
	/*union
	{
		unsigned int number;
		const char *string;
	};*/
	string text;
	static const char *types[14];
public:
	LexemType Type() const
	{
		return type;
	}
	const string &String() const
	{
		return text;
	}
	const char *StringType() const
	{
		return types[type];
	}
};

class Scanner
{
private:
	Scanner(const Scanner&);
	Scanner &operator=(const Scanner&);
public:	
	Scanner(istream&, ostream&);
	~Scanner();
	
	bool operator>>(Lexem&);
private:
	istream &input;
	ostream &errors;
	enum {H, A, B, C, D, E, F, G, I, J, K, L, M, N, P, Q} state;
};

#endif
