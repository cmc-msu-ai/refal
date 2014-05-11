#include "Scanner.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "Lexem.h"

const char *Scanner::errors_table[] =
{
	"Unforeseen character",
	"Unforeseen character after plus",
	"Unclosed string",
	"Wrong first character of qualifier",
	"Unclosed qualifier",
	"Wrong first character of label",
	"Unclosed number",
	"Unclosed label"
};

const Scanner::uchar Scanner::RETURN_LEXEM = 0x01;
const Scanner::uchar Scanner::UNGET        = 0x02;

Scanner::Scanner(istream &i, ostream &e)
	: input(i), errors(e), status(true), state(Z), line(1), offset(0)
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
	
	init_switch_table();
	
	state = Z;
}

Scanner::~Scanner()
{
}

void Scanner::unget()
{
	input.unget();
	if(input.peek() == '\n')
		line--;
	else if(offset > 0)
		offset--;
}

void Scanner::error(Error e)
{
	status = false;
	errors << std::setw(4) << line << ":" << std::setw(3) << offset
				<< ": Error: " << errors_table[e] << std::endl;
}

bool Scanner::operator>>(Lexem &lexem)
{
	if(!input.good())
		return false;
	
	for(char curr = input.get(); input.good(); curr = input.get())
	{
		if(curr == '\n')
		{
			line++;
			offset = 0;
		}
		else
			offset++;
	
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
								uchar mode, const TActionFunc &action)
{
	const int fnumber = switch_functions.size();
	switch(mode)
	{
	case 0x0:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return false;
		});
		break;
	case RETURN_LEXEM:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return true;
		});
		break;
	case UNGET:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			unget();
			return false;
		});
		break;
	case RETURN_LEXEM | UNGET:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			unget();
			return true;
		});
		break;
	default:
		break;
	}
	
	for(int j = 0; j < 256; j++)
		if(cond.test(j))
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(const CharSet &cond, State ns, uchar mode)
{
	const int fnumber = switch_functions.size();
		switch(mode)
	{
	case 0x0:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			return false;
		});
		break;
	case RETURN_LEXEM:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			return true;
		});
		break;
	case UNGET:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			unget();
			return false;
		});
		break;
	case RETURN_LEXEM | UNGET:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			unget();
			return true;
		});
		break;
	default:
		break;
	}
	for(int j = 0; j < 256; j++)
		if(cond.test(j))
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(State ns, uchar mode, const TActionFunc &action)
{
	const int fnumber = switch_functions.size();
	switch(mode)
	{
	case 0x0:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return false;
		});
		break;
	case RETURN_LEXEM:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			return true;
		});
		break;
	case UNGET:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			unget();
			return false;
		});
		break;
	case RETURN_LEXEM | UNGET:
		switch_functions.push_back(
		[&, ns, action](Lexem &l, char c)
		{
			action(l, c);
			state = ns;
			unget();
			return true;
		});
		break;
	default:
		break;
	}

	for(int j = 0; j < 256; j++)
		if(switch_table[state][j] == 0)
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(State ns, uchar mode)
{
	const int fnumber = switch_functions.size();
	switch(mode)
	{
	case 0x0:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			return false;
		});
		break;
	case RETURN_LEXEM:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			return true;
		});
		break;
	case UNGET:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			unget();
			return false;
		});
		break;
	case RETURN_LEXEM | UNGET:
		switch_functions.push_back(
		[&, ns](Lexem &l, char c)
		{
			state = ns;
			unget();
			return true;
		});
		break;
	default:
		break;
	}

	for(int j = 0; j < 256; j++)
		if(switch_table[state][j] == 0)
			switch_table[state][j] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(uchar mode, const TFinallyActionFunc &action)
{
	const int fnumber = finally_functions.size();
	if(mode & RETURN_LEXEM)
	{
		finally_functions.push_back(
		[&, action](Lexem &l)
		{
			action(l);
			return true;
		});
	}
	else
	{
		finally_functions.push_back(
		[&, action](Lexem &l)
		{
			action(l);
			return false;
		});
	}
	finally_table[state] = fnumber;
	return *this;
}

Scanner &Scanner::operator()(uchar mode)
{
	finally_table[state] = (mode & RETURN_LEXEM) ? 1 : 0;
	return *this;
}

void Scanner::init_switch_table()
{
	static const CharSet letter = CharSet('a', 'z') + CharSet('A', 'Z');
	static const CharSet digit = CharSet('0', '9');
	(*this)
	#define ACTION [&](Lexem &l, char c) -> void
	#define FINALLY_ACTION [&](Lexem &l) -> void
	[Z]
		(CharSet("\r\n"), Z)
		(CharSet(" \t"), Y, 0x0, ACTION
			{
				l = new SimpleLexem(line, offset, SPACE);
			})
		(CharSet('*'), X)
		(H, UNGET)
	[Y]
		(CharSet("\r\n"), Z)
		(CharSet(" \t"), Y)
		(CharSet('*'), X)
		(H, RETURN_LEXEM | UNGET)
	[X]
		(CharSet("\r\n"), Z)
		(X)
	[H]
		(CharSet("\r\n"), Z, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, LINE_FEED);
			})
		(CharSet(" \t"), A, 0x0, ACTION
			{
				l = new SimpleLexem(line, offset, SPACE);
			})
		(letter + digit + CharSet('_'), H, RETURN_LEXEM, ACTION
			{
				l = new LetterOrDigitLexem(line, offset, c);
			})
		(CharSet('+'), B)
		(CharSet(','), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, COMMA);
			})
		(CharSet('('), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, LEFT_PAREN);
			})
		(CharSet(')'), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, RIGHT_PAREN);
			})
		(CharSet('<'), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, LESS);
			})
		(CharSet('>'), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, GREAT);
			})
		(CharSet('='), H, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, EQUALITY);
			})
		(CharSet(':'), C, 0x0, ACTION
			{
				l = new StringLexem(line, offset, QUALIFIER);
			})
		(CharSet('/'), E)
		(CharSet('\''), S, 0x0, ACTION
			{
				l = new StringLexem(line, offset, STRING);
			})
		(CharSet('*'), X, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, LINE_FEED);
			})
		(H, 0x0, ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
	[A]
		(CharSet("\r\n"), H)
		(CharSet(" \t"), A)
		(CharSet('+'), B)
		(CharSet('*'), X, RETURN_LEXEM, ACTION
			{
				l = new SimpleLexem(line, offset, LINE_FEED);
			})
		(H, RETURN_LEXEM | UNGET)
	[B]
		(CharSet(" \t"), B)
		(CharSet("\r\n"), H)
		(B, 0x0, ACTION
			{
				error(UNFORESEEN_CHARACTER_AFTER_PLUS);
			})
	[C]
		(letter + CharSet('_'), D, UNGET)
		(C, 0x0, ACTION
			{
				error(WRONG_FIRST_QUALIFIERS_CHAR);
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_QUALIFIER);
			})
	[D]
		(letter + digit + CharSet('_'), D, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet(':'), H, RETURN_LEXEM)
		(H, RETURN_LEXEM | UNGET, ACTION
			{
				error(UNCLOSED_QUALIFIER);
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_QUALIFIER);
			})
	[E]
		(digit, F, 0x0, ACTION
			{
				l = new NumberLexem(line, offset, c-'0');
			})
		(letter + CharSet('_'), G, UNGET, ACTION
			{
				l = new StringLexem(line, offset, LABEL);
			})
		(H, UNGET, ACTION
			{
				error(WRONG_FIRST_LABELS_CHAR);
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_LABEL);
			})
	[F]
		(digit, F, 0x0, ACTION
			{
				NumberLexem& nl = static_cast<NumberLexem&>(*l);
				nl.value = nl.value * 10 + c-'0';
			})
		(CharSet('/'), H, RETURN_LEXEM)
		(H, RETURN_LEXEM | UNGET, ACTION
			{
				error(UNCLOSED_NUMBER);
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_NUMBER);
			})
	[G]
		(letter + digit + CharSet('_'), G, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(CharSet('/'), H, RETURN_LEXEM)
		(H, RETURN_LEXEM | UNGET, ACTION
			{
				error(UNCLOSED_LABEL);
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_LABEL);
			})
	[S]
		(CharSet('\''), H, RETURN_LEXEM)
		(CharSet('\\'), I)
		(CharSet(0, 31) + CharSet(127), S, 0x0, ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
		(S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[I]
		(CharSet('0', '7'), J, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c-'0';
			})
		(CharSet('x'), K)
		(CharSet("aA"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\a';
			})
		(CharSet("bB"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\b';
			})
		(CharSet("fF"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\f';
			})
		(CharSet("nN"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\n';
			})
		(CharSet("rR"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\r';
			})
		(CharSet("tT"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\t';
			})
		(CharSet("vV"), S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += '\v';
			})
		(CharSet('\0'), I, 0x0, ACTION
			{
				error(UNFORESEEN_CHARACTER);
			})
		(S, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c;
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[J]
		(CharSet('0', '7'), J, 0x0, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 3) + c-'0';
			})
		(S, UNGET)
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[K]
		(CharSet('a', 'f'), L, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += 10+c-'a';
			})
		(CharSet('A', 'F'), L, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += 10+c-'A';
			})
		(CharSet('0', '9'), L, 0x0, ACTION
			{
				static_cast<StringLexem&>(*l).value += c-'0';
			})
		(S, UNGET, ACTION
			{
				static_cast<StringLexem&>(*l).value += 'x';
			})
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	[L]
		(CharSet('a', 'f'), L, 0x0, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + 10+c-'a';
			})
		(CharSet('A', 'F'), L, 0x0, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + 10+c-'A';
			})
		(CharSet('0', '9'), L, 0x0, ACTION
			{
				char &e = static_cast<StringLexem&>(*l).value.back();
				e = (e << 4) + c-'0';
			})
		(S, UNGET)
		(0x0, FINALLY_ACTION
			{
				error(UNCLOSED_STRING);
			})
	#undef ACTION
	#undef FINALLY_ACTION
	;
}
