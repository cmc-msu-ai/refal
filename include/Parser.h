#ifndef _PARSER__H_
#define _PARSER__H_

#include <iostream>
#include "Scanner.h"

using std::istream;
using std::ostream;

class Parser
{
private:
	Parser(const Parser&);
	Parser& operator=(const Parser&);
public:
	Parser(istream&, ostream&);
	~Parser();
	
	bool Status()
	{
		return status;
	}
private:
	bool status;
	Scanner scanner;
	Lexem lexem;
	
	void getlexem();
	
	void WS();
	bool I(string&);
	void S();
	void A();
	void D();
};

#endif
