#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>

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
};

#endif
