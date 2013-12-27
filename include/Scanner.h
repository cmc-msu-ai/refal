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
	
	bool operator>>(Lexem&);
private:
	istream &input;
	ostream &errors;
		
	void inc_line();
	
	enum State
	{
		H, A, B, C, D, E, F, G, I, J, K, L, M, N, P, Q, Z, Y, X, STATES_COUNT
	};
	typedef function<bool(Lexem&, char c)> TSwitchFunc;
	typedef function<void(Lexem&, char c)> TActionFunc;
	typedef function<void(Lexem&)> TFinallyActionFunc;
	typedef function<bool(Lexem&)> TFinallyFunc;
	
	State state;
	int line_count, sym_count;
	unsigned int switch_table[STATES_COUNT][256];
	unsigned int finally_table[STATES_COUNT];
	vector<TSwitchFunc> switch_functions;
	vector<TFinallyFunc> finally_functions;
	
	Scanner &operator[](State);
	Scanner &operator()(const CharSet&, State, bool, const TActionFunc&);
	Scanner &operator()(const CharSet&, State, bool = false);
	Scanner &operator()(State, bool, const TActionFunc&);
	Scanner &operator()(bool, const TFinallyActionFunc&);
	Scanner &operator()(bool = false);
};

#endif
