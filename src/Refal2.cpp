#include "Refal2.h"
#include <iostream>

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
	for(char c = input.get(); input.good(); c = input.get())
	{
		text += c;
	}
	status = true;
	return status;
}

void Refal2::Run(istream &input, ostream &output) const
{
	if(!status)
		return;

	output << text;
}
