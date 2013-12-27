#include "Refal2.h"
#include <iostream>
#include "Parser.h"

Refal2::Refal2()
	: status(false)
{
}

Refal2::Refal2(istream &input, ostream &errors)
{
	Load(input, errors);
}

Refal2::~Refal2()
{
}

bool Refal2::Load(istream &input, ostream &errors)
{
	Parser parser(input, errors);
	status = parser.Status();
	return status;
}

void Refal2::Run(istream &input, ostream &output) const
{
	if(!status)
		return;
}
