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
	std::string word;
	std::vector<char> string;
};

struct CToken {
	int line;
	int position;
	TTokenType type;
	CTokenValue value;
};

class CTokenQueue : public std::queue<CToken> {
public:
	void clear();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
