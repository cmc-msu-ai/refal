#include "Scanner.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "Lexem.h"

const char *Scanner::errors_table[] =
{
	"Unforeseen character",
	"Unclosed string",
	"wrong line feed in string",
	"Wrong first character of qualifier",
	"Unclosed qualifier",
	"Wrong first character of label",
	"Unclosed label"
};

Scanner::Scanner(istream &i, ostream &e)
	: input(i), errors(e), status(true), state(H), line_count(1), sym_count(0)
{
	for(int i = 0; i < STATES_COUNT; ++i)
	{
		finally_table[i] = 0;
		for(int j = 0; j < 256; j++)
			switch_table[i][j] = 0;
	}
	
	switch_functions.push_back([](Lexem &l, char c){return false;});
	finally_functions.push_back([](Lexem &l){return false;});
	finally_functions.push_back([](Lexem &l){return true;});
	
	static const CharSet num = CharSet('0', '9');
	static const CharSet idstart = CharSet('a', 'z') +
									CharSet('A', 'Z') + CharSet('_');
	static const CharSet idsym = idstart + num;
	
	(*this)
	#define ACTION [&](Lexem &l, char c) -> void
	#define FINALLY_ACTION [&](Lexem &l) -> void
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
				unget();
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
				unget();
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
				unget();
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
				unget();
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
				error(UNFORESEEN_CHARACTER);
			})
	[D]
		(CharSet('\''), B, true)
		(CharSet('\\'), E, false)
		(CharSet(0, 31) + CharSet(127), D, false, ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
		(CharSet('\r'), V, false, ACTION
			{
				error(WRONG_LINE_FEED_IN_STRING);
				inc_line();
				static_cast<StringLexem&>(*l).value += '\r';
			})
		(CharSet('\n'), D, false, ACTION
			{
				error(WRONG_LINE_FEED_IN_STRING);
				inc_line();
				static_cast<StringLexem&>(*l).value += '\n';
			})
		(D, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(false, FINALLY_ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
	[V]
		(CharSet('\n'), V, false, ACTION
			{
				error(WRONG_LINE_FEED_IN_STRING);
				static_cast<StringLexem&>(*l).value += '\n';
			})
		(D, false, ACTION
			{
				unget();
			})
	[E]
		(CharSet("\r\n"), D, false, ACTION
			{
				unget();
			})
		(CharSet('0', '7'), N, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c-'0';
			})
		(CharSet('x'), P, false)
		(CharSet('\0'), E, false, ACTION
			{
				error(UNFORESEEN_CHARACTER);
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
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[N]
		(CharSet('0', '7'), N, false, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 3) + c-'0';
			})
		(D, false, ACTION
			{
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
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
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
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
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[C]
		(idsym, C, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(B, true, ACTION
			{
				unget();
			})
		(true)
	[F]
		(idstart, G, false, ACTION
			{
				l = new StringLexem(QUALIFIER);
				unget();
			})
		(B, false, ACTION
			{
				error(WRONG_FIRST_QUALIFIERS_CHAR);
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_QUALIFIER);
			})
	[G]
		(idsym, G, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet(':'), B, true)
		(B, true, ACTION
			{
				error(UNCLOSED_QUALIFIER);
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_QUALIFIER);
			})
	[I]
		(idstart, K, false, ACTION
			{
				l = new StringLexem(LABEL);
				unget();
			})
		(num, J, false, ACTION
			{
				l = new NumberLexem(NUMBER, c-'0');
			})
		(B, false, ACTION
			{
				error(WRONG_FIRST_LABELS_CHAR);
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
	[J]
		(num, J, false, ACTION
			{
				NumberLexem& nl = static_cast<NumberLexem&>(*l);
				nl.value = nl.value * 10 + c-'0';
			})
		(CharSet('/'), B, true)
		(B, true, ACTION
			{
				error(UNCLOSED_LABEL);
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_LABEL);
			})
	[K]
		(idsym, K, false, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet('/'), B, true)
		(B, true, ACTION
			{
				error(UNCLOSED_LABEL);
				unget();
			})
		(false, FINALLY_ACTION
			{
				error(UNCLOSED_LABEL);
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
				error(UNFORESEEN_CHARACTER);
			})
		(false, FINALLY_ACTION
			{
				error(UNFORESEEN_CHARACTER);
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
				unget();
			})
	[Y]
		(CharSet(" \t\n"), M)
		(CharSet('\r'), Y, false, ACTION
			{
				inc_line();
			})
		(B, false, ACTION
			{
				unget();
			})
	#undef ACTION
	#undef FINALLY_ACTION
	;
	state = H;
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

void Scanner::unget()
{
	sym_count--;
	input.unget();
}

void Scanner::error(Error e)
{
	status = false;
	errors << std::setw(4) << line_count << ":" << std::setw(3) << sym_count
				<< ": Error: " << errors_table[e] << std::endl;
}

bool Scanner::operator>>(Lexem &lexem)
{
	if(!input.good())
		return false;

	for(char curr = input.get(); input.good(); curr = input.get())
	{
		sym_count++;
		if(switch_functions[switch_table[state][int((unsigned char)curr)]]
					(lexem, curr))
			return true;
	}
	if(finally_functions[finally_table[state]](lexem))
		return true;
	
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

Scanner &Scanner::operator()(bool lex_done, const TFinallyActionFunc &action)
{
	const int fnumber = finally_functions.size();
	finally_functions.push_back(
		[&, lex_done, action](Lexem &l)
		{
			action(l);
			return lex_done;
		});
	finally_table[state] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(bool lex_done)
{
	finally_table[state] = lex_done ? 1 : 0;
	return *this;
}
