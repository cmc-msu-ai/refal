#pragma once

#include <Refal2.h>

template<class T>
class CFastSet {
public:
	typedef std::set<T> CSet;

	CFastSet(): elementsSize(0), elements(0) {}
	CFastSet(const CSet&);
	CFastSet(const CFastSet&);
	~CFastSet() { Empty(); }

	CFastSet& operator=(const CFastSet&);
	CFastSet& operator=(const CSet&);

	inline void Empty();
	bool IsEmpty() const { return ( elementsSize == 0 ); }
	inline void Swap(CFastSet* swapWith);
	inline void Move(CFastSet* moveTo);

	int GetSize() { return elementsSize; }
	void GetSet(CSet* set) const { *set = *this; }
	inline operator CSet() const;

	CFastSet& operator*=(const CFastSet&);
	CFastSet& operator+=(const CFastSet&);
	CFastSet& operator-=(const CFastSet&);	

	bool Has(const T element) const;

private:
	int elementsSize;
	T* elements;
};

template<class T>
CFastSet<T>::CFastSet(const CSet& set):
	elementsSize( set.size() ),
	elements(new T[elementsSize])
{
	int i = 0;
	for( typename CSet::const_iterator j = set.begin(); j != set.end(); ++j ) {
		elements[i] = *j;
		i++;
	}
}

template<class T>
CFastSet<T>::CFastSet( const CFastSet& set ):
	elementsSize( 0 ),
	elements( 0 )
{
	*this = set;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator=( const CFastSet& set )
{
	if( this != &set ) {
		if( elementsSize != set.elementsSize ) {
			delete[] elements;
			elementsSize = set.elementsSize;
			elements = new T[elementsSize];
		}
		for( int i = 0; i < elementsSize; i++ ) {
			elements[i] = set.elements[i];
		}
	}
	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator=(const CSet& set)
{
	delete[] elements;
	elementsSize = set.size();
	elements = new T[elementsSize];
	
	int i = 0;
	for( typename CSet::const_iterator j = set.begin(); j != set.end(); ++j ) {
		elements[i] = *j;
		i++;
	}

	return *this;
}

template<class T>
inline void CFastSet<T>::Empty()
{
	elementsSize = 0;
	delete[] elements;
	elements = 0;
}

template<class T>
inline void CFastSet<T>::Swap(CFastSet<T>* swapWith)
{
	std::swap( elementsSize, swapWith->elementsSize );
	std::swap( elements, swapWith->elements );
}

template<class T>
inline void CFastSet<T>::Move(CFastSet<T>* moveTo)
{
	if( this != moveTo ) {
		moveTo->Empty();
		Swap( moveTo );
	}
}

template<class T>
inline CFastSet<T>::operator CSet() const
{
	return CSet(elements, elements + elementsSize);
}

template<class T>
CFastSet<T>& CFastSet<T>::operator+=(const CFastSet& set)
{
	CSet a;
	GetSet( &a );
	CSet b;
	set.GetSet( &b );
	CSet result;

	std::set_union( a.begin(), a.end(), b.begin(), b.end(),
		std::inserter( result, result.end() ) );

	*this = result;

	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator*=(const CFastSet& set)
{
	CSet a;
	GetSet( &a );
	CSet b;
	set.GetSet( &b );
	CSet result;

	std::set_intersection( a.begin(), a.end(), b.begin(), b.end(),
		std::inserter( result, result.end() ) );

	*this = result;

	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator-=(const CFastSet& set)
{
	CSet a;
	GetSet( &a );
	CSet b;
	set.GetSet( &b );
	CSet result;

	std::set_difference( a.begin(), a.end(), b.begin(), b.end(),
		std::inserter( result, result.end() ) );

	*this = result;

	return *this;
}

template<class T>
bool CFastSet<T>::Has(const T element) const
{
	CSet tmpSet;
	GetSet( &tmpSet );
	return ( tmpSet.count( element ) == 1 );
}
