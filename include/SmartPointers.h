#pragma once

#include <Common.h>

template<class ObjectType>
class CScopedPtr {
public:
	CScopedPtr( ObjectType* objectPtr = 0 );
	CScopedPtr( CScopedPtr& otherPtr ) { otherPtr.Move( *this ); }
	~CScopedPtr();

	CScopedPtr& operator=( CScopedPtr& otherPtr ) { otherPtr.Move( *this ); }
	CScopedPtr& operator=( ObjectType* newObjectPtr );

	void Reset( ObjectType* newObjectPtr = 0 );
	bool IsNull() const { return ( ptr == 0 ); }
	void Move( CScopedPtr& moveTo );
	void Swap( CScopedPtr& swapWith );

	ObjectType* Get() const { return ptr; }
	ObjectType* Detach();

	operator ObjectType*() const { return ptr; }
	ObjectType& operator*() const;
	ObjectType* operator->() const;

private:
	ObjectType* ptr;

	CScopedPtr( const CScopedPtr& );
	CScopedPtr operator=( const CScopedPtr& );
};

template<class ObjectType>
inline CScopedPtr<ObjectType>::CScopedPtr( ObjectType* objectPtr ):
	ptr( objectPtr )
{
}

template<class ObjectType>
inline CScopedPtr<ObjectType>::~CScopedPtr()
{
	Reset();
}

template<class ObjectType>
CScopedPtr<ObjectType>& CScopedPtr<ObjectType>::operator=( ObjectType*
	newObjectPtr )
{
	CScopedPtr tmp( newObjectPtr );
	tmp.Move( *this );
}

template<class ObjectType>
inline void CScopedPtr<ObjectType>::Reset( ObjectType* newObjectPtr )
{
	if( ptr != 0 ) {
		delete ptr;
	}
	ptr = newObjectPtr;
}

template<class ObjectType>
inline void CScopedPtr<ObjectType>::Move( CScopedPtr& moveTo )
{
	if( &moveTo != this ) {
		assert( ptr == 0 || ptr != moveTo.ptr );
		moveTo.Reset();
		Swap( moveTo );
	}
}

template<class ObjectType>
inline void CScopedPtr<ObjectType>::Swap( CScopedPtr& swapWith )
{
	std::swap( ptr, swapWith.ptr );
}

template<class ObjectType>
inline ObjectType* CScopedPtr<ObjectType>::Detach()
{
	ObjectType* tmp = ptr;
	ptr = 0;
	return tmp;
}

template<class ObjectType>
inline ObjectType& CScopedPtr<ObjectType>::operator*() const
{
	assert( ptr != 0 );
	return *ptr;
}

template<class ObjectType>
inline ObjectType* CScopedPtr<ObjectType>::operator->() const
{
	assert( ptr != 0 );
	return ptr;
}
