#include <Refal2.h>

namespace Refal2 {

// definition of global variables
CLabelTable LabelTable;

// tools function
void MakeLower( std::string& data )
{
	std::transform( data.begin(), data.end(), data.begin(), ::tolower );
}

} // end of namespace Refal2
