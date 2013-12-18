#include "Scanner.h"
#include <iostream>
#include <string>

Lexem::Lexem(Type t, const string &s)
	: type(t), text(s)
{
}

Lexem::Lexem(const Lexem &l)
{
	operator=(l);
}
Lexem &Lexem::operator=(const Lexem &l)
{
	type = l.type;
	text = l.text;
	return *this;
}

bool Scanner::operator<<(int c)
{
	if(c == END_OF_FILE)
		return false;
	lex = c;
	return true;
}

void Scanner::operator>>(Lexem &l)
{
	l = Lexem(Lexem::STRING, string()+lex);
}
