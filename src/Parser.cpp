#include "Parser.h"
#include <iostream>
#include <iomanip>
#include "Scanner.h"

using std::istream;
using std::ostream;
using std::cout;
using std::endl;
using std::setw;

const char *types[15] =
{
	"INDEFINITELY",
	"LETTER_OR_DIGIT",
	"LEFT_PAREN",
	"RIGHT_PAREN",
	"COMMA",
	"EQUALITY",
	"LESS",
	"GREAT",
	"QUALIFIER",
	"LABEL",
	"NUMBER",
	"STRING",
	"LINE_FEED",
	"SPACE"
};

Parser::Parser(istream &input, ostream &errors)
	: status(true)
{
	Scanner scanner(input, errors);
	Lexem lexem;
	while(scanner >> lexem)
	{
		cout << setw(4) << lexem->line << ":" <<
				setw(3) << lexem->offset << "|";
		switch(lexem->type)
		{
		case STRING:
		case QUALIFIER:
		case LABEL:
			cout << types[lexem->type] << "|" << 
				static_cast<StringLexem*>(lexem.get())->value << "|" << endl;
			break;
		case NUMBER:
			cout << types[lexem->type] << ":" <<
				static_cast<NumberLexem*>(lexem.get())->value << "." << endl;
			break;
		case LETTER_OR_DIGIT:
			cout << types[lexem->type] << "[" <<
				static_cast<LetterOrDigitLexem*>(lexem.get())->value <<
					"]" << endl;
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
