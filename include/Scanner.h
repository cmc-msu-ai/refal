#ifndef _SCANNER__H_
#define _SCANNER__H_

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "CharSet.h"
#include "Lexem.h"

using std::istream;
using std::ostream;
using std::string;
using std::vector;
using std::function;

class Scanner
{
private:
	Scanner(const Scanner&);
	Scanner &operator=(const Scanner&);
public:	
	Scanner(istream&, ostream&);
	~Scanner();
	
	bool Status()
	{
		return status;
	}
	
	bool operator>>(Lexem&);
private:
	istream &input;
	ostream &errors;
	
	typedef function<bool(Lexem&, char c)> TSwitchFunc;
	typedef function<void(Lexem&, char c)> TActionFunc;
	typedef function<void(Lexem&)> TFinallyActionFunc;
	typedef function<bool(Lexem&)> TFinallyFunc;
	typedef unsigned char uchar;
	
	enum State
	{
		Z, Y, X, H, A, B, C, D, E, F, G, S, I, J, K, L, STATES_COUNT
	};	
	enum Error
	{
		UNFORESEEN_CHARACTER,
		UNFORESEEN_CHARACTER_AFTER_PLUS,
		UNCLOSED_STRING,
		WRONG_FIRST_QUALIFIERS_CHAR,
		UNCLOSED_QUALIFIER,
		WRONG_FIRST_LABELS_CHAR,
		UNCLOSED_NUMBER,
		UNCLOSED_LABEL,
		ERRORS_COUNT
	};
	static const uchar RETURN_LEXEM, UNGET;
	
	bool status;
	State state;
	static const char *errors_table[ERRORS_COUNT];
	unsigned int line, offset;
	vector<TSwitchFunc> switch_functions;
	vector<TFinallyFunc> finally_functions;
	unsigned int switch_table[STATES_COUNT][256];
	unsigned int finally_table[STATES_COUNT];
	
	Scanner &operator[](State);
	Scanner &operator()(const CharSet&, State, uchar, const TActionFunc&);
	Scanner &operator()(const CharSet&, State, uchar = 0x0);
	Scanner &operator()(State, uchar, const TActionFunc&);
	Scanner &operator()(State, uchar = 0x0);
	Scanner &operator()(uchar, const TFinallyActionFunc&);
	Scanner &operator()(uchar = 0x0);
	
	void init_switch_table();
	void line_offset_count(char c);
	void unget();
	void error(Error);
};

#endif
