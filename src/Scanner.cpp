#include "Scanner.h"
#include <iostream>
#include <string>
#include "Lexem.h"

Scanner::Scanner(istream &i, ostream &e)
	: input(i), errors(e), state(H), line_count(1), sym_count(0)
{
	for(int i = 0; i < STATES_COUNT; ++i)
		for(int j = 0; j < 256; j++)
			switch_table[i][j] = 0;
	
	switch_functions.push_back([](Lexem &l, char c){return false;});
	
	static const CharSet num = CharSet('0', '9');
	static const CharSet idstart = CharSet('a', 'z') +
									CharSet('A', 'Z') + CharSet('_');
	static const CharSet idsym = idstart + num;
	
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
		(B, true, ACTION
			{
				l = new SimpleLexem(PARAGRAPH);
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
		(CharSet(','), B, true, ACTION
			{
				l = new SimpleLexem(COMMA);
			})
		(CharSet('<'), B, true, ACTION
			{
				l = new SimpleLexem(LESS);
			})
		(CharSet('>'), B, true, ACTION
			{
				l = new SimpleLexem(GREAT);
			})
		(CharSet('('), B, true, ACTION
			{
				l = new SimpleLexem(LEFT_PAREN);
			})
		(CharSet(')'), B, true, ACTION
			{
				l = new SimpleLexem(RIGHT_PAREN);
			})
		(CharSet('='), B, true, ACTION
			{
				l = new SimpleLexem(EQUALITY);
			})
		(idstart, C, false, ACTION
			{
				l = new StringLexem(IDENTIFIER);
				input.unget();
			})
		(CharSet(':'), F)
		(CharSet('/'), I)
		(CharSet('*'), X)
		(CharSet('+'), L)
		(CharSet('\''), D, false, ACTION
			{
				l = new StringLexem(STRING);
			})
		(B, false, ACTION
			{
				/* error */
				std::cout << c;
			})
	[D]
		(CharSet('\''), B, true)
		(D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
	[C]
		(idsym, C, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(B, true, ACTION
			{
				input.unget();
			})
	[F]
		(idstart, G, false, ACTION
			{
				l = new StringLexem(QUALIFIER);
				input.unget();
			})
		(F, false, ACTION
			{
				/* error */
			})
	[G]
		(idsym, G, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet(':'), B, true)
		(G, false, ACTION
			{
				/* error */
			})
	[I]
		(idstart, K, false, ACTION
			{
				/* label */
			})
		(num, J, false, ACTION
			{
				/* number */
			})
		(I, false, ACTION
			{
				/* error */
			})
	[J]
		(num, J, false, ACTION
			{
				/* number */
			})
		(CharSet('/'), B, false, ACTION
			{
				/* made number */
			})
		(J, false, ACTION
			{
				/* error */
			})
	[K]
		(idsym, K, false, ACTION
			{
				/* label */
			})
		(CharSet('/'), B, false, ACTION
			{
				/* made label */
			})
		(B, false, ACTION
			{
				/* error */
			})
	[X]
		(CharSet('\r'), Z, false, ACTION
			{
				inc_line();
			})
		(CharSet('\n'), H, false, ACTION
			{
				inc_line();
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
	bool status = input.good();
	for(char curr = input.get(); input.good(); curr = input.get())
	{
		sym_count++;
		if(switch_functions[switch_table[state][int((unsigned char)curr)]]
					(lexem, curr))
			break;
	}
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
