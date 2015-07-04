#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TTokenType {
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

struct CTokenValue {
	TNumber number;
	std::string text;

	void Swap( CTokenValue& swapWith );
	void Move( CTokenValue& moveTo );
};

struct CToken {
	int line;
	int position;
	TTokenType type;
	CTokenValue value;

	void Swap( CToken& swapWith );
	void Move( CToken& moveTo );
};

//-----------------------------------------------------------------------------

inline void CTokenValue::Swap( CTokenValue& swapWith )
{
	std::swap( number, swapWith.number );
	std::swap( text, swapWith.text );
}

inline void CTokenValue::Move( CTokenValue& moveTo )
{
	moveTo.number = number;
	moveTo.text.clear();
	std::swap( text, moveTo.text );
}

inline void CToken::Swap( CToken& swapWith )
{
	std::swap( line, swapWith.line );
	std::swap( position, swapWith.position );
	std::swap( type, swapWith.type );
	value.Swap( swapWith.value );
}

inline void CToken::Move( CToken& moveTo )
{
	moveTo.line = line;
	moveTo.position = position;
	moveTo.type = type;
	value.Move( moveTo.value );
}

class CTokenQueue : public std::queue<CToken> {
public:
	void clear();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
