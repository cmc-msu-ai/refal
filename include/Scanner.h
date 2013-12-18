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
	enum LexemType
	{
		INDEFINITELY, COMMA, LESS, GREAT, NUMBER, STRING, QUALIFIER, LABEL,
		PARAGRAPH, LINE_FEED, LEFT_PAREN, IDENTIFIER, EQUALITY, RIGHT_PAREN
	};
public:
	Lexem();
	~Lexem();
	
	Lexem(const Lexem &l);
	Lexem &operator=(const Lexem &l);
private:
	LexemType type;
	/*union
	{
		unsigned int number;
		const char *string;
	};*/
	string text;
public:
	LexemType Type() const
	{
		return type;
	}
	const string &String() const
	{
		return text;
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
};

#endif
