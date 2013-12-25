#ifndef _SCANNER__H_
#define _SCANNER__H_

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "CharSet.h"

using std::istream;
using std::ostream;
using std::string;
using std::vector;
using std::function;

class BasicLexem
{
public:
	BasicLexem();
	virtual ~BasicLexem() = 0;
};

class Lexem
{
	friend class Scanner;
public:
	Lexem();
	~Lexem();
	
	Lexem(const Lexem &l);
	Lexem &operator=(const Lexem &l);
public:
	enum LexemType
	{
		INDEFINITELY, COMMA, LESS, GREAT, RIGHT_PAREN,
		LINE_FEED, LEFT_PAREN, PARAGRAPH, EQUALITY,
		NUMBER,
		STRING, QUALIFIER, LABEL, IDENTIFIER
	};
private:
	LexemType type;
	/*union
	{
		unsigned int number;
		const char *string;
	};*/
	string text;
	static const char *types[14];
public:
	LexemType Type() const
	{
		return type;
	}
	const string &String() const
	{
		return text;
	}
	const char *StringType() const
	{
		return types[type];
	}
};

class Scanner
{
private:
	Scanner(const Scanner&);
	Scanner &operator=(const Scanner&);
public:	
	Scanner(istream&, ostream&);
	~Scanner();
	
	bool operator>>(Lexem&);
private:
	istream &input;
	ostream &errors;
		
	void inc_line();
	
	enum State
	{
		H, A, B, C, D, E, F, G, I, J, K, L, M, N, P, Q, Z, Y, STATES_COUNT
	};	
	typedef function<bool(Lexem&, char c)> TSwitchFunc;
	typedef function<void(Lexem&, char c)> TActionFunc;
	
	State state;
	int line_count, sym_count;
	unsigned int switch_table[STATES_COUNT][256];
	vector<TSwitchFunc> switch_functions;
	
	Scanner &operator[](State);
	Scanner &operator()(const CharSet&, State, bool, const TActionFunc&);
	Scanner &operator()(const CharSet&, State, bool = false);
	Scanner &operator()(State, bool, const TActionFunc&);
};

#endif
