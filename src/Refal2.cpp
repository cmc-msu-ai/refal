#include <Refal2.h>

namespace Refal2 {

// tools function
void MakeLower( std::string& data )
{
	std::transform( data.begin(), data.end(), data.begin(), ::tolower );
}

} // end of namespace Refal2
