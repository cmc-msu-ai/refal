#include "Scanner.h"
#include <iostream>
#include <string>

/* Lemem */
Lexem::Lexem()
	: type(INDEFINITELY)
{
}

Lexem::~Lexem()
{
}

Lexem::Lexem(const Lexem &l)
{
	operator=(l);
}

Lexem &Lexem::operator=(const Lexem &l)
{
	return *this;
}

/* Scanner */
Scanner::Scanner(istream &i, ostream &e)
	: input(i), errors(e)
{
}

Scanner::~Scanner()
{
}

bool Scanner::operator>>(Lexem &lexem)
{
	bool status = input.good();
	for(char c = input.get(); input.good(); c = input.get())
	{
		lexem.text += c;
	}
	return status;
}
