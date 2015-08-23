#include <Refal2.h>

namespace Refal2 {

// tools function
void MakeLower( std::string& data )
{
	std::transform( data.begin(), data.end(), data.begin(), ::tolower );
}

bool CompareCharNoCase( const char a, const char b )
{
    return ( ::tolower( a ) == ::tolower( b ) );
}

bool CompareNoCase( const std::string& str1, const std::string& str2 )
{
	return std::lexicographical_compare( str1.begin(), str1.end(),
		str2.begin(), str2.end(), CompareCharNoCase );
}

} // end of namespace Refal2
