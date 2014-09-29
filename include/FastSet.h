#pragma once

#include <algorithm>
#include <set>
#include <string.h>

template<class T>
class CFastSet {
public:
	typedef std::set<T> TSet;

	CFastSet();
	CFastSet(const TSet&);
	CFastSet(const CFastSet&);
	~CFastSet();

	CFastSet& operator=(const CFastSet&);
	CFastSet& operator=(const TSet&);

	CFastSet& operator*=(const CFastSet&);
	CFastSet& operator-=(const CFastSet&);

	void Swap(CFastSet&);

	const T* Find(const T element) const;

	void GetSet(TSet&) const;
	operator TSet() const;

private:
	T* elements;
	int size;
};

template<class T>
CFastSet<T>::CFastSet():
	elements(0),
	size(0)
{
}

template<class T>
CFastSet<T>::CFastSet(const TSet& set):
	elements(new T[set.size()]),
	size(set.size())
{
	int i = 0;
	typename TSet::const_iterator j = set.begin();
	while( j != set.end() ) {
		elements[i] = *j;
		++i;
		++j;
	}
}

template<class T>
CFastSet<T>::CFastSet(const CFastSet& set):
	elements(new T[set.size]),
	size(set.size)
{
	memcpy(elements, set.elements, size * sizeof(T));
}

template<class T>
CFastSet<T>::~CFastSet()
{
	delete[] elements;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator=(const CFastSet& set)
{
	if( this != &set ) {
		delete[] elements;
		size = set.size;
		elements = new T[size];
		memcpy(elements, set.elements, size * sizeof(T));
	}

	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator=(const TSet& set)
{
	delete[] elements;
	size = set.size();
	elements = new T[size];
	
	int i = 0;
	typename TSet::const_iterator j = set.begin();
	while( j != set.end() ) {
		elements[i] = *j;
		++i;
		++j;
	}

	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator*=(const CFastSet& set)
{
	if( this != &set ) {
		int j = 0;
		int i = 0;
		T* src = elements;
		T* dst = elements;

		while( i < size && j < set.size ) {
			if( set.elements[j] < src[0] ) {
				++j;
				while( j < set.size && set.elements[j] < src[0] ) {
					++j;
				}
				if( j == set.size ) {
					break;
				}
			}

			if( dst != src ) {
				memmove(dst, src, sizeof(T));
			}

			if( ! (src[0] < set.elements[j]) ) {
				++dst;
			}

			++src;
			++i;
		}

		size = dst - elements;
	}

	return *this;
}

template<class T>
CFastSet<T>& CFastSet<T>::operator-=(const CFastSet& set)
{
	if( this != &set ) {
		int j = 0;
		int i = 0;
		T* src = elements;
		T* dst = elements;

		while( i < size && j < set.size ) {
			if( set.elements[j] < src[0] ) {
				++j;
				while( j < set.size && set.elements[j] < src[0] ) {
					++j;
				}
				if( j == set.size ) {
					break;
				}
			}

			if( dst != src ) {
				memmove(dst, src, sizeof(T));
			}

			if( src[0] < set.elements[j] ) {
				++dst;
			}

			++src;
			++i;
		}

		size = dst - elements;
	}

	return *this;
}

template<class T>
void CFastSet<T>::Swap(CFastSet<T>& set)
{
	int tmp = size;
	size = set.size;
	set.size = tmp;

	T* tmpe = elements;
	elements = set.elements;
	set.elements = tmpe;
}

template<class T>
void CFastSet<T>::GetSet(TSet& set) const
{
	set = TSet(elements, elements + size);
}

template<class T>
CFastSet<T>::operator TSet() const
{
	return TSet(elements, elements + size);
}

template<class T>
const T* CFastSet<T>::Find(const T element) const
{
	int l = 0;
	int r = size;

	if( size == 0 || element < elements[0] ) {
		return 0;
	}

	while( l < r )
	{
		int i = (l + r) / 2;
		
		if( elements[i] < element ) {
			l = i + 1;
		} else {
			r = i;
		}
	}

	if( r < size && !( element < elements[r] ) ) {
		return ( elements + r );
	}

	return 0;
}

