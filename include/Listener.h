#pragma once

namespace Refal2 {

template<class ILISTENER>
class CListener {
public:
	explicit CListener(ILISTENER* _listener = 0): listener(_listener) {}

	bool HasListener() const { return ( listener != 0 ); }
	inline void SetListener(ILISTENER* newListener) { listener = newListener; }
	ILISTENER* GetListener() const { return listener; }
	
private:
	ILISTENER* listener;
};

} // end of namespace Refal2
