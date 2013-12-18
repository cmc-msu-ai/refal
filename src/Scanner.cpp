#include "Scanner.h"
#include <iostream>
#include <string>

/* Lemem */
const char *Lexem::types[14] =
{
	"INDEFINITELY", "COMMA", "LESS", "GREAT",
	"NUMBER", "STRING", "QUALIFIER", "LABEL",
	"PARAGRAPH", "LINE_FEED", "LEFT_PAREN",
	"IDENTIFIER", "EQUALITY", "RIGHT_PAREN"
};

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
	: input(i), errors(e), state(H)
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
