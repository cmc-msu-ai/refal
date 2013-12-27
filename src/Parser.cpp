#include "Parser.h"
#include <iostream>
#include "Scanner.h"

using std::istream;
using std::ostream;
using std::cout;
using std::endl;

const char *types[14] =
{
	"INDEFINITELY",
	"COMMA",
	"LESS",
	"GREAT",
	"LEFT_PAREN",
	"RIGHT_PAREN",
	"LINE_FEED",
	"PARAGRAPH",
	"EQUALITY",
	"NUMBER",
	"STRING",
	"QUALIFIER",
	"LABEL",
	"IDENTIFIER"
};

Parser::Parser(istream &input, ostream &errors)
	: status(true)
{
	Scanner scanner(input, errors);
	Lexem lexem;
	while(scanner >> lexem)
	{
		switch(lexem->type)
		{
		case STRING:
		case QUALIFIER:
		case LABEL:
		case IDENTIFIER:
			cout << types[lexem->type] << "|" << 
				static_cast<StringLexem*>(lexem.get())->value << "|" << endl;
			break;
		case NUMBER:
			cout << types[lexem->type] << ":" <<
				static_cast<NumberLexem*>(lexem.get())->value << "." << endl;
			break;
		default:
			cout << types[lexem->type] << endl;
			break;
		}
		//std::cin.get();
	}
	status = scanner.Status();
}

Parser::~Parser()
{
}
