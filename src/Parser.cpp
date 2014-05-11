#include "Parser.h"
#include <iostream>
#include <string>
#include <iomanip>
#include "Scanner.h"

using std::istream;
using std::ostream;
using std::cout;
using std::endl;
using std::setw;
using std::string;

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
	: status(true), scanner(input, errors)
{
#if 1
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
#else
	S();
#endif
}

Parser::~Parser()
{
}

void Parser::getlexem()
{
	scanner >> lexem;
}

void Parser::WS()
{
	if(lexem->type == SPACE)
		getlexem();
}

bool Parser::I(string &i)
{
	int line = lexem->line, offset = lexem->offset;
	
	i.clear();
	for(; lexem->type == LETTER_OR_DIGIT; getlexem())
		i += tolower(static_cast<LetterOrDigitLexem*>(lexem.get())->value);
	
	if(i.empty())
	{
		/* error: identifier expected */
	}
	else if(i.front() >= '0' && i.front() <= '9')
	{
		/* error: identifier can't begin with digit */
	}
	else
		return true;
	
	return false;
}

void Parser::S()
{
	/* S -> I SPACE `start` LINE_FEED A */
	string i;
	I(i);
	if(I(i) && i != "start")
	{
		/* error: `start` expected */
	}
}

void Parser::A()
{
	/* A -> SPACE D | I K A */
	if(lexem->type == SPACE)
		D();
	else
	{
		I();
		K();
		A();
	}
}

void Parser::D()
{
}
