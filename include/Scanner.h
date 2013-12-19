#ifndef _SCANNER__H__
#define _SCANNER__H__

#include <iostream>
#include <string>

using std::istream;
using std::ostream;
using std::string;

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
		INDEFINITELY, COMMA, LESS, GREAT, NUMBER, STRING, QUALIFIER, LABEL,
		PARAGRAPH, LINE_FEED, LEFT_PAREN, IDENTIFIER, EQUALITY, RIGHT_PAREN
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
	char curr;
	string lex;
	
	enum State
	{
		H, A, B, C, D, E, F, G, I, J, K, L, M, N, P, Q, STATES_COUNT
	};
	State state;
	
	typedef bool (Scanner::*TSwitchFunction)();
	TSwitchFunction switch_table[STATES_COUNT][256];
	
	void def_rule(State s, const char *event, TSwitchFunction func);
	void def_rule(State s, TSwitchFunction func);
	void clear_switch_table();
	
	bool Hspace(); bool Hnewline(); bool Hrest();
	
	bool Aspace(); bool Anewline(); bool Arest();
	
	bool Bspace(); bool Bnewline(); bool Bidstart(); bool Bdelim();
	bool Bslash(); bool Bplus(); bool Bcolon(); bool Bquote(); bool Brest();
	
	bool Cidsym(); bool Crest();
	
	bool Dbackslash(); bool Dquote(); bool Dctrls(); bool Drest();
	
	bool E07(); bool Ex(); bool Eabfnrtv(); bool Erest();
	
	bool Fidstart(); bool Frest();
	
	bool Gidsym(); bool Gcolon(); bool Grest();
	
	bool I09(); bool Iidstart(); bool Irest();
	
	bool J09(); bool Jslash(); bool Jrest();
	
	bool Kidsym(); bool Kslash(); bool Krest();
	
	bool Lspace(); bool Lnewline(); bool Lrest();
	
	bool Mspace_newline(); bool Mrest();
	
	bool N07(); bool Nrest();
	
	bool Phex(); bool Prest();
	
	bool Qhex(); bool Qrest();
};

#endif
