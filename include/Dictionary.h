#pragma once

#include <Common.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const int InvalidDictionaryIndex = -1;
const int DefaultDictionaryCapacity = 512;

template<class Data, class Key = std::string>
class CDictionary {
public:
	CDictionary( int initialCapacity = DefaultDictionaryCapacity );
	~CDictionary() { Free(); }

	void Free();
	void Empty();
	void Swap( CDictionary& swapWith );
	void Move( CDictionary& moveTo );
	bool IsEmpty() { return ( Size() == 0 ); }
	int Size() const { return size; }
	int Capacity() const { return capacity; }
	void SetCapacity( int newCapacity );
	int AddKey( const Key& key );
	const Key& GetKey( int index ) const;
	int FindKey( const Key& key ) const;
	Data& GetData( int index ) const;

private:
	int size;
	int capacity;
	typedef std::map<Key, int> CKeyToIndex;
	typedef typename CKeyToIndex::const_iterator CKeyToIndexConstIterator;
	struct CKeyDataPair {
		CKeyToIndexConstIterator key;
		Data data;

		CKeyDataPair( const CKeyToIndexConstIterator& _key ): key( _key ) {}
	};
	CKeyDataPair* keyDataPairs;
	CKeyToIndex keyToIndex;

	// auxiliary functions
	void grow( int newCapacity );

	CDictionary( const CDictionary& );
	CDictionary& operator=( const CDictionary& );
};

//-----------------------------------------------------------------------------

template<class Data, class Key>
CDictionary<Data, Key>::CDictionary( int initialCapacity ):
	size( 0 ),
	capacity( 0 ),
	keyDataPairs( 0 )
{
	SetCapacity( initialCapacity );
}

template<class Data, class Key>
void CDictionary<Data, Key>::Free()
{
	Empty();
	capacity = 0;
	::operator delete( keyDataPairs );
	keyDataPairs = 0;
}

template<class Data, class Key>
void CDictionary<Data, Key>::Empty()
{
	keyToIndex.clear();
	for( int i = 0; i < size; i++ ) {
		keyDataPairs[i].~CKeyDataPair();
	}
	size = 0;
}

template<class Data, class Key>
void CDictionary<Data, Key>::Swap( CDictionary& swapWith )
{
	std::swap( size, swapWith.size );
	std::swap( capacity, swapWith.capacity );
	std::swap( keyDataPairs, swapWith.keyDataPairs );
	keyToIndex.swap( swapWith.keyToIndex );
}

template<class Data, class Key>
void CDictionary<Data, Key>::Move( CDictionary& moveTo )
{
	if( &moveTo != this ) {
		Swap( moveTo );
		Free();
	}
}

template<class Data, class Key>
void CDictionary<Data, Key>::SetCapacity( int newCapacity )
{
	grow( newCapacity );
}

template<class Data, class Key>
int CDictionary<Data, Key>::AddKey( const Key& key )
{
	typedef std::pair<typename CKeyToIndex::iterator, bool> CPair;
	CPair insert = keyToIndex.insert( std::make_pair( key, size ) );
	const int result = insert.first->second;
	if( insert.second ) {
		size++;
		if( capacity < size ) {
			grow( capacity * 2 );
		}
		new( keyDataPairs + result )CKeyDataPair( insert.first );
	}
	return result;
}

template<class Data, class Key>
const Key& CDictionary<Data, Key>::GetKey( int index ) const
{
	assert( index >= 0 && index < size );
	return keyDataPairs[index].key->first;
}

template<class Data, class Key>
int CDictionary<Data, Key>::FindKey( const Key& key ) const
{
	typename CKeyToIndex::const_iterator i = keyToIndex.find( key );
	if( i != keyToIndex.end() ) {
		return i->second;
	}
	return InvalidDictionaryIndex;
}

template<class Data, class Key>
Data& CDictionary<Data, Key>::GetData( int index ) const
{
	assert( index >= 0 && index < size );
	return const_cast<Data&>( keyDataPairs[index].data );
}

template<class Data, class Key>
void CDictionary<Data, Key>::grow( int newCapacity )
{
	assert( newCapacity > capacity );
	capacity = newCapacity;
	void* memory = ::operator new( capacity * sizeof( CKeyDataPair ) );
	::memcpy( memory, keyDataPairs, size * sizeof( CKeyDataPair ) );
	::operator delete( keyDataPairs );
	keyDataPairs = static_cast<CKeyDataPair*>( memory );
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
