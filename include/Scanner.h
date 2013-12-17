#ifndef _SCANNER__H__
#define _SCANNER__H__

#include <string>

using namespace std;

class Lexem
{
public:
	enum Type
	{
		COMMA, LESS, GREAT, LEFT_PAREN, RIGHT_PAREN, EQUALITY, PARAGRAPH,
		LINE_FEED, NUMBER, LABEL, QUALIFIER, STRING, IDENTIFIER
	};
private:
	Type type;
	string text;
public:
	Lexem(Type t, const string &s);
	
	Lexem(const Lexem &l);
	Lexem &operator=(const Lexem &l);
	
	Type GetType() const
	{
		return type;
	}
	const string &GetText() const
	{
		return text;
	}
};

class Scanner
{
public:
	enum
	{
		END_OF_FILE = -1
	};
private:
	char lex;
public:	
	bool operator<<(int c);
	void operator>>(Lexem &l);
};

#endif
