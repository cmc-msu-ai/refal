#include <Refal2.h>

namespace Refal2 {

// tools function
void MakeLower( std::string& data )
{
	std::transform( data.begin(), data.end(), data.begin(), ::tolower );
}

bool CompareNoCase( const char a, const char b )
{
	return ( ::tolower( a ) == ::tolower( b ) );
}

bool CompareNoCase( const std::string& str1, const std::string& str2 )
{
	if( str1.length() != str2.length() ) {
		return false;
	}
	std::string::const_iterator i = str1.begin();
	std::string::const_iterator j = str2.begin();
	for( ; i != str1.end(); ++i, ++j ) {
		if( !CompareNoCase( *i, *j ) ) {
			return false;
		}
	}
	assert( j == str2.end() );
	return true;
}

} // end of namespace Refal2
