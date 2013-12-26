#include "Lexem.h"

Lexem::Lexem(Lexem &lexem)
	: l(lexem.l)
{
	lexem.l = 0;
}
Lexem &Lexem::operator=(Lexem &lexem)
{
	if(lexem.l == l)
		return *this;
	
	freep();
	l = lexem.l;
	lexem.l = 0;
	return *this;
}
Lexem &Lexem::operator=(SimpleLexem *lexem)
{
	if(lexem == l)
		return *this;
	
	freep();
	l = lexem;
	return *this;
}
Lexem::~Lexem()
{
	freep();
}
void Lexem::freep()
{
	if(!l)
		return;

	switch(l->type)
	{
	case STRING:
	case QUALIFIER:
	case LABEL:
	case IDENTIFIER:
		delete static_cast<StringLexem*>(l);
		break;
	case NUMBER:
		delete static_cast<NumberLexem*>(l);
		break;
	default:
		delete l;
		break;
	}
	l = 0;
}
