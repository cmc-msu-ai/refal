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
		(CharSet('\r'), Z, true, ACTION
			{
				l = new SimpleLexem(LINE_FEED);
				inc_line();
			})
		(CharSet('\n'), H, true, ACTION
			{
				l = new SimpleLexem(LINE_FEED);
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
		(idsym, C, false, ACTION
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
			})
	[D]
		(CharSet('\''), B, true)
		(CharSet('\\'), E, false)
		(CharSet(0, 31) + CharSet(127), D, false, ACTION
			{
				/* error */
			})
		(D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
	[E]
		(CharSet('0', '7'), N, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c-'0';
			})
		(CharSet('x'), P, false)
		(CharSet('\0'), E, false, ACTION
			{
				/* error */
			})
		(CharSet("aA"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\a';
			})
		(CharSet("bB"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\b';
			})
		(CharSet("fF"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\f';
			})
		(CharSet("nN"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\n';
			})
		(CharSet("rR"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\r';
			})
		(CharSet("tT"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\t';
			})
		(CharSet("vV"), D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\v';
			})
		(D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
	[N]
		(CharSet('0', '7'), N, false, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 3) + c-'0';
			})
		(D, false, ACTION
			{
				input.unget();
			})
	[P]
		(CharSet('a', 'f'), Q, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += 10+c-'a';
			})
		(CharSet('A', 'F'), Q, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += 10+c-'a';
			})
		(CharSet('0', '9'), Q, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c-'0';
			})
		(D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += 'x';
				input.unget();
			})
	[Q]
		(CharSet('a', 'f'), Q, false, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + 10+c-'a';
			})
		(CharSet('A', 'F'), Q, false, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + 10+c-'A';
			})
		(CharSet('0', '9'), Q, false, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + c-'0';
			})
		(D, false, ACTION
			{
				input.unget();
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
				l = new StringLexem(LABEL);
				input.unget();
			})
		(num, J, false, ACTION
			{
				l = new NumberLexem(NUMBER, c-'0');
			})
		(I, false, ACTION
			{
				/* error */
			})
	[J]
		(num, J, false, ACTION
			{
				NumberLexem& nl = static_cast<NumberLexem&>(*l);
				nl.value = nl.value * 10 + c-'0';
			})
		(CharSet('/'), B, true)
		(J, false, ACTION
			{
				/* error */
			})
	[K]
		(idsym, K, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet('/'), B, true)
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
	for(char curr = input.get(); input.good(); curr = input.get())
	{
		sym_count++;
		if(switch_functions[switch_table[state][int((unsigned char)curr)]]
					(lexem, curr))
			return true;
	}
	return false;
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
