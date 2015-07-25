#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

bool AreTokenWordsEqual( const CToken& word1, const CToken& word2 )
{
	std::string text1 = word1.word;
	std::string text2 = word2.word;
	MakeLower( text1 );
	MakeLower( text2 );
	return ( text1 == text2 );
}

void CTokenQueue::clear()
{
	CTokenQueue emptyQueue;
	std::swap( *this, emptyQueue );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
