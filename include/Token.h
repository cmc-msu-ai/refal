#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CToken

enum TTokenType {
	TT_None,
	TT_Word,
	TT_Blank,
	TT_Comma,
	TT_Equal,
	TT_Label,
	TT_Number,
	TT_String,
	TT_LineFeed,
	TT_Qualifier,
	TT_LeftParen,
	TT_RightParen,
	TT_LeftBracket,
	TT_RightBracket
};

struct CToken {
	TTokenType type;
	int line;
	int position;
	TNumber number;
	std::string word;

	CToken(): type( TT_None ) {}

	void Swap( CToken& swapWith );
	void Move( CToken& moveTo );

	bool IsNone() const { return ( type == TT_None ); }
};

//-----------------------------------------------------------------------------

inline void CToken::Swap( CToken& swapWith )
{
	std::swap( type, swapWith.type );
	std::swap( line, swapWith.line );
	std::swap( position, swapWith.position );
	std::swap( number, swapWith.number );
	word.swap( swapWith.word );
}

inline void CToken::Move( CToken& moveTo )
{
	moveTo.type = type;
	moveTo.line = line;
	moveTo.position = position;
	moveTo.number = number;
	moveTo.word.clear();
	word.swap( moveTo.word );
}

//-----------------------------------------------------------------------------
// CTokenQueue

class CTokenQueue : public std::queue<CToken> {
public:
	void clear();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
