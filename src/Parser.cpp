#include "Parser.h"
#include <iostream>
#include "Scanner.h"

using std::istream;
using std::ostream;
using std::cout;
using std::endl;

Parser::Parser(istream &input, ostream &errors)
{
	Scanner scanner(input, errors);
	Lexem lexem;
	while(scanner >> lexem)
	{
		cout << lexem.String() << endl;
	}
}

Parser::~Parser()
{
}
