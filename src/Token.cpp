#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CTokenQueue

void CTokenQueue::clear()
{
	CTokenQueue emptyQueue;
	std::swap( *this, emptyQueue );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
