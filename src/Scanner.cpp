#include "Scanner.h"
#include <iostream>
#include <string>

#define ABC			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NUM09		"0123456789"
#define NUM07		"01234567"
#define HEX			"abcdefABCDEF" NUM09
#define SPACETAB	"\t "
#define NEWLINE		"\r\n"
#define IDSTART		ABC "_"
#define IDSYM		IDSTART NUM09
#define BACKSLASH	"\\"
#define DELIM		"()<>,="
#define SLASH		"/"
#define PLUS		"+"
#define COLON		":"
#define QUOTE		"\'"
#define CTRLS		"\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\
\21\23\24\25\26\27\30\31\32\33\34\35\36\37\177"

/* Lemem */
const char *Lexem::types[14] =
{
	"INDEFINITELY", "COMMA", "LESS", "GREAT",
	"NUMBER", "STRING", "QUALIFIER", "LABEL",
	"PARAGRAPH", "LINE_FEED", "LEFT_PAREN",
	"IDENTIFIER", "EQUALITY", "RIGHT_PAREN"
};

Lexem::Lexem()
	: type(INDEFINITELY)
{
}

Lexem::~Lexem()
{
}

Lexem::Lexem(const Lexem &l)
{
	operator=(l);
}

Lexem &Lexem::operator=(const Lexem &l)
{
	return *this;
}

/* Scanner */
Scanner::Scanner(istream &i, ostream &e)
	: input(i), errors(e), state(H), line_count(1), sym_count(0)
{
	for(int i = 0; i < STATES_COUNT; ++i)
		for(int j = 0; j < 256; j++)
			switch_table[i][j] = 0;
	
	switch_functions.push_back([](Lexem &l, char c){return false;});
	
	(*this)
	#define ACTION [&](Lexem &l, char c) -> void
	[H]
		(CharSet('\n'), H, false, ACTION
			{
				inc_line();
			})
		(CharSet('\r'), Z, false, ACTION
			{
				inc_line();
			})
		(CharSet(" \t"), A)
		(B, false, ACTION
			{
				input.unget();
			})
	[Z]
		(CharSet('\r'), Z, false, ACTION
			{
				inc_line();
			})
		(CharSet('\n'), H)
		(CharSet(" \t"), A)
		(B, false, ACTION
			{
				input.unget();
			})
	[A]
		(CharSet('\r'), Z, false, ACTION
			{
				inc_line();
			})
		(CharSet('\n'), H, false, ACTION
			{
				inc_line();
			})
		(CharSet(" \t"), A)
		(B, false, ACTION
			{
				/* made lexem */
				input.unget();
			})
	[B]
		(CharSet('\r'), Z, false, ACTION
			{
				inc_line();
			})
		(CharSet('\n'), H, false, ACTION
			{
				inc_line();
			})
		(CharSet(" \t"), B)
		(CharSet('+'), L)		
		(B, false, ACTION
			{
				std::cout << c;
			})
	[L]
		(CharSet(" \t"), L)
		(CharSet('\r'), Y, false, ACTION
			{
				inc_line();
			})
		(CharSet('\n'), M, false, ACTION
			{
				inc_line();
			})
		(L, false, ACTION
			{
				/* error */
			})
	[M]
		(CharSet(" \t"), M)
		(CharSet('\n'), M, false, ACTION
			{
				inc_line();
			})
		(CharSet('\r'), Y, false, ACTION
			{
				inc_line();
			})
		(B, false, ACTION
			{
				input.unget();
			})
	[Y]
		(CharSet(" \t\n"), M)
		(CharSet('\r'), Y, false, ACTION
			{
				inc_line();
			})
		(B, false, ACTION
			{
				input.unget();
			})			
	#undef ACTION
	;
}

Scanner::~Scanner()
{
}

void Scanner::inc_line()
{
	line_count++;
	sym_count = 0;
	std::cout << std::endl << line_count << std::endl;
}

bool Scanner::operator>>(Lexem &lexem)
{
	Lexem lex;
	bool status = input.good();
	for(char curr = input.get(); input.good(); curr = input.get())
	{
		sym_count++;
		switch_functions[switch_table[state][unsigned(curr)]](lex, curr);
	}
	std::cout << "+++++++++++++++++++++++++++" << std::endl;
	return status;
}

Scanner &Scanner::operator[](State s)
{
	state = s;
	return *this;
}

Scanner &Scanner::operator()(const CharSet &cond, State ns,
								bool lex_done, const TActionFunc &action)
{
	const int fnumber = switch_functions.size();
	switch_functions.push_back(
		[&, ns, lex_done, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return lex_done;
		});
	for(int j = 0; j < 256; j++)
		if(cond.test(j))
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(const CharSet &cond, State ns, bool lex_done)
{
	const int fnumber = switch_functions.size();
	switch_functions.push_back(
		[&, ns, lex_done](Lexem &l, char c)
		{
			state = ns;
			return lex_done;
		});
	for(int j = 0; j < 256; j++)
		if(cond.test(j))
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(State ns, bool lex_done, const TActionFunc &action)
{
	const int fnumber = switch_functions.size();
	switch_functions.push_back(
		[&, ns, lex_done, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return lex_done;
		});
	for(int j = 0; j < 256; j++)
		if(switch_table[state][j] == 0)
			switch_table[state][j] = fnumber;
	return *this;
}
