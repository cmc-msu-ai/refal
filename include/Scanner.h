#ifndef _SCANNER__H__
#define _SCANNER__H__

enum LexemsTypes
{
	LEXEM_TYPE_IDENT,   /* identificator */
	LEXEM_TYPE_ENTER,   /* line feed */
	LEXEM_TYPE_PADDING, /* padding at the beginning of the line */
	LEXEM_TYPE_NUMBER,  /* sequence of figure */
	LEXEM_TYPE_STRING,  /* quoted string */
	LEXEM_TYPE_LPAREN,  /* ( */
	LEXEM_TYPE_RPAREN,  /* ) */
	LEXEM_TYPE_LESS,    /* < */
	LEXEM_TYPE_GREAT,   /* > */
	LEXEM_TYPE_SLASH,   /* / */
	LEXEM_TYPE_COLON,   /* : */
	LEXEM_TYPE_COMMA,   /* , */
	LEXEM_TYPE_QUOTE,   /* ' */
	LEXEM_TYPE_EQUAL,   /* = */
};

class Lexem
{
};

class Scanner
{
public:
	Scanner();
	
	
};

#endif
