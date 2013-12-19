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
	: input(i), errors(e), state(H)
{
	clear_switch_table();
	
	def_rule(H, SPACETAB, &Scanner::Hspace);
	def_rule(H, NEWLINE, &Scanner::Hnewline);
	def_rule(H, &Scanner::Hrest);
	
	def_rule(A, SPACETAB, &Scanner::Aspace);
	def_rule(A, NEWLINE, &Scanner::Anewline);
	def_rule(A, &Scanner::Arest);
	
	def_rule(B, SPACETAB, &Scanner::Bspace);
	def_rule(B, NEWLINE, &Scanner::Bnewline);
	def_rule(B, IDSTART, &Scanner::Bidstart);
	def_rule(B, DELIM, &Scanner::Bdelim);
	def_rule(B, SLASH, &Scanner::Bslash);
	def_rule(B, PLUS, &Scanner::Bplus);
	def_rule(B, COLON, &Scanner::Bcolon);
	def_rule(B, QUOTE, &Scanner::Bquote);
	def_rule(B, &Scanner::Brest);
	
	def_rule(C, IDSYM, &Scanner::Cidsym);
	def_rule(C, &Scanner::Crest);
	
	def_rule(D, BACKSLASH, &Scanner::Dbackslash);
	def_rule(D, QUOTE, &Scanner::Dquote);
	def_rule(D, CTRLS, &Scanner::Dctrls);
	def_rule(D, &Scanner::Drest);
	
	def_rule(E, NUM07, &Scanner::E07);
	def_rule(E, "x", &Scanner::Ex);
	def_rule(E, "abfnrtv", &Scanner::Eabfnrtv);
	def_rule(E, &Scanner::Erest);
	
	def_rule(F, IDSTART, &Scanner::Fidstart);
	def_rule(F, &Scanner::Frest);
	
	def_rule(G, IDSYM, &Scanner::Gidsym);
	def_rule(G, COLON, &Scanner::Gcolon);
	def_rule(G, &Scanner::Grest);
	
	def_rule(I, NUM09, &Scanner::I09);
	def_rule(I, IDSTART, &Scanner::Iidstart);
	def_rule(I, &Scanner::Irest);
	
	def_rule(J, NUM09, &Scanner::J09);
	def_rule(J, SLASH, &Scanner::Jslash);
	def_rule(J, &Scanner::Jrest);
	
	def_rule(K, IDSYM, &Scanner::Kidsym);
	def_rule(K, SLASH, &Scanner::Kslash);
	def_rule(K, &Scanner::Krest);

	def_rule(L, SPACETAB, &Scanner::Lspace);
	def_rule(L, NEWLINE, &Scanner::Lnewline);
	def_rule(L, &Scanner::Lrest);

	def_rule(M, SPACETAB NEWLINE, &Scanner::Mspace_newline);
	def_rule(M, &Scanner::Mrest);
	
	def_rule(N, NUM07, &Scanner::N07);
	def_rule(N, &Scanner::Nrest);
	
	def_rule(P, HEX, &Scanner::Phex);
	def_rule(P, &Scanner::Prest);
	
	def_rule(Q, HEX, &Scanner::Qhex);
	def_rule(Q, &Scanner::Qrest);
}

Scanner::~Scanner()
{
}

bool Scanner::operator>>(Lexem &lexem)
{
	lex.clear();
	bool status = input.good();
	for(curr = input.get(); input.good(); curr = input.get())
	{
		/*std::cout << state << "|" << curr << "|" << std::endl;*/
		if(	(this->*(switch_table[state][int(curr)]))() )
		{
			lexem.text = lex;
			break;
		}
	}
	return status;
}

void Scanner::clear_switch_table()
{
	for(int i = 0; i < STATES_COUNT; i++)
		for(int j = 0; j < 256; j++)
			switch_table[i][j] = 0;
}

void Scanner::def_rule(State s, const char *event, TSwitchFunction func)
{
	for(int i = 0; event[i] != '\0'; i++)
		switch_table[s][int(event[i])] = func;
}

void Scanner::def_rule(State s, TSwitchFunction func)
{
	for(int j = 0; j < 256; j++)
		if(!switch_table[s][j])
			switch_table[s][j] = func;
}

/* H -> ... */
bool Scanner::Hspace()
{
	state = A;
	return false;
}
bool Scanner::Hnewline()
{
	return false;
}
bool Scanner::Hrest()
{
	state = B;
	input.unget();
	return false;
}
/* A -> ... */
bool Scanner::Aspace()
{
	return false;
}
bool Scanner::Anewline()
{
	state = H;
	return false;
}
bool Scanner::Arest()
{
	lex = "TAB ";
	input.unget();
	state = B;
	return true;
}
/* B -> ... */
bool Scanner::Bspace()
{
	return false;
}
bool Scanner::Bnewline()
{
	lex = "NEWLINE";
	state = H;
	return true;
}
bool Scanner::Bidstart()
{
	input.unget();
	state = C;
	return false;
}
bool Scanner::Bdelim()
{
	lex = string("DELIM:") + curr + " ";
	state = B;
	return true;
}
bool Scanner::Bslash()
{
	state = I;
	return false;
}
bool Scanner::Bplus()
{
	state = L;
	return false;
}
bool Scanner::Bcolon()
{
	state = F;
	return false;
}
bool Scanner::Bquote()
{
	state = D;
	return false;
}
bool Scanner::Brest()
{
	/* ERROR */
	return false;
}
/* C -> ... */
bool Scanner::Cidsym()
{
	lex += curr;
	return false;
}
bool Scanner::Crest()
{
	input.unget();
	state = B;
	return true;
}
/* D -> ... */
bool Scanner::Dbackslash()
{
	state = E;
	return false;
}
bool Scanner::Dquote()
{
	lex = string("STRING:'") + lex + "' ";
	state = B;
	return true;
}
bool Scanner::Dctrls()
{
	/* ERROR */
	return false;
}
bool Scanner::Drest()
{
	lex += curr;
	return false;
}
/* E -> ... */
bool Scanner::E07()
{
	state = N;
	return false;
}
bool Scanner::Ex()
{
	state = P;
	return false;
}
bool Scanner::Eabfnrtv()
{
	return false;
}
bool Scanner::Erest()
{
	lex += curr;
	return false;
}
/* F -> ... */
bool Scanner::Fidstart()
{
	input.unget();
	state = G;
	return false;
}
bool Scanner::Frest()
{
	/* ERROR */
	return false;
}
/* G -> ... */
bool Scanner::Gidsym()
{
	lex += curr;
	return false;
}
bool Scanner::Gcolon()
{
	/* ERROR */
	lex = string("SPEC:") + lex + " ";
	return true;
}
bool Scanner::Grest()
{
	/* ERROR */
	return false;
}
/* I -> ... */
bool Scanner::I09()
{
	input.unget();
	state = J;
	return false;
}
bool Scanner::Iidstart()
{
	input.unget();
	state = K;
	return false;
}
bool Scanner::Irest()
{
	/* ERROR */
	return false;
}
/* J -> ... */
bool Scanner::J09()
{
	lex += curr;
	return false;
}
bool Scanner::Jslash()
{
	lex = string("NUMBER:") + lex + " ";
	state = B;
	return true;
}
bool Scanner::Jrest()
{
	/* ERROR */
	return false;
}
/* K -> ... */
bool Scanner::Kidsym()
{
	lex += curr;
	return false;
}
bool Scanner::Kslash()
{
	lex = string("ID:") + lex + " ";
	state = B;
	return true;
}
bool Scanner::Krest()
{
	/* ERROR */
	return false;
}
/* L -> ... */
bool Scanner::Lspace()
{
	return false;
}
bool Scanner::Lnewline()
{
	state = M;
	return false;
}
bool Scanner::Lrest()
{
	/* ERROR */
	return false;
}
/* M -> ... */
bool Scanner::Mspace_newline()
{
	return false;
}
bool Scanner::Mrest()
{
	input.unget();
	state = B;
	return false;
}
/* N -> ... */
bool Scanner::N07()
{
	lex += curr;
	return false;
}
bool Scanner::Nrest()
{
	state = D;
	return true;
}
/* P -> ... */
bool Scanner::Phex()
{
	lex += curr;
	return false;
}
bool Scanner::Prest()
{
	state = D;
	return true;
}
/* Q -> ... */
bool Scanner::Qhex()
{
	lex += curr;
	return false;
}
bool Scanner::Qrest()
{
	state = D;
	return true;
}
