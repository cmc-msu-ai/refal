#pragma once

#include <utility>

namespace Refal2 {

template<class T>
class CNodeList;

template<class T>
class CNode : public T {
	friend class CNodeList<T>;
	
public:
	CNode(): prev(0), next(0) {}
	explicit CNode(const T& value): T( value ), prev(0), next(0) {}

	CNode* Next() { return next; }
	const CNode* Next() const { return next; }
	
	CNode* Prev() { return prev; }
	const CNode* Prev() const { return prev; }
	
private:
	CNode* prev;
	CNode* next;
};

template<class T>
class CNodeList {
public:
	typedef CNode<T> TNode;

	CNodeList(): first(0), last(0) {}
	CNodeList(TNode* first, TNode* last);
	
	~CNodeList() { Empty(); }
	
	void Assign(TNode* _first, TNode* _last);
	
	void Empty();
	bool IsEmpty() { return ( first == 0 ); }
	
	inline void Swap(CNodeList* swapWith);
	inline void Move(CNodeList* moveTo);
	
	inline TNode* DetachFirst();
	void RemoveFirst() { free( DetachFirst() ); }
	TNode* GetFirst() { return first; }
	const TNode* GetFirst() const { return first; }
	
	inline TNode* DetachLast();
	void RemoveLast() { free( DetachLast() ); }
	TNode* GetLast() { return last; }
	const TNode* GetLast() const { return last; }
	
	TNode* InsertBefore(TNode* nodeBefore, const T& value);
	
	TNode* InsertAfter(TNode* nodeAfter, const T& value)
		{ return InsertAfter( nodeAfter, alloc( value ) ); }
	TNode* InsertAfter(TNode* nodeAfter, TNode* node)
		{ InsertAfter( nodeAfter, node, node ); return node; }
	void InsertAfter(TNode* nodeAfter, TNode* fromNode, TNode* toNode);
	
	void Detach(TNode* node) { Detach( node, node ); }
	void Detach(TNode* fromNode, TNode* toNode);
	
	void Remove(TNode* node) { Detach( node ); free( node ); }
	void Remove(TNode* fromNode, TNode* toNode);
	
	void Move(TNode* nodeAfter, TNode* node) { InsertAfter( nodeAfter, node ); }
	void Move(TNode* nodeAfter, TNode* fromNode, TNode* toNode)
		{ InsertAfter( nodeAfter, fromNode, toNode ); }
	
	void Copy(TNode* nodeAfter, TNode* node) { Copy( nodeAfter, node, node ); }
	void Copy(TNode* nodeAfter, TNode* fromNode, TNode* toNode);
	
	static void Duplicate(const TNode* fromNode, const TNode* toNode,
		TNode** fromNodeCopy, TNode** toNodeCopy);
	
	TNode* Append(const T& value);

private:
	CNodeList(const CNodeList&);
	CNodeList& operator=(const CNodeList&);

	static TNode* alloc(const T& value);
	static void free(TNode* node);
	
	TNode* first;
	TNode* last;
};

template<class T>
inline void CNodeList<T>::Swap(CNodeList* swapWith)
{
	std::swap( first, swapWith->first );
	std::swap( last, swapWith->last );
}

template<class T>
inline void CNodeList<T>::Move(CNodeList* moveTo)
{
	if( this != moveTo ) {
		moveTo->Empty();
		Swap( moveTo );
	}
}

template<class T>
inline typename CNodeList<T>::TNode* CNodeList<T>::DetachFirst()
{
	TNode* detachedNode = GetFirst();
	Detach( detachedNode );
	return detachedNode;
}

template<class T>
inline typename CNodeList<T>::TNode* CNodeList<T>::DetachLast()
{
	TNode* detachedNode = GetLast();
	Detach( detachedNode );
	return detachedNode;
}

template<class T>
CNodeList<T>::CNodeList(TNode* _first, TNode* _last):
	first( _first ), last( _last )
{
	assert( _first != 0 && _last != 0 );
}

template<class T>
void CNodeList<T>::Assign(TNode* _first, TNode* _last)
{
	Empty();
	first = _first;
	last = _last;
}

template<class T>
void CNodeList<T>::Empty()
{
	if( first != 0 ) {
		Remove(first, last);
	}
	first = 0;
	last = 0;
}

template<class T>
typename CNodeList<T>::TNode* CNodeList<T>::InsertBefore(TNode* nodeBefore,
	const T& value)
{
	TNode* newNode = alloc( value );
	
	newNode->next = nodeBefore;
	newNode->prev = nodeBefore->prev;
	if( nodeBefore == first ) {
		first = newNode;
	} else {
		nodeBefore->prev->next = newNode;
	}
	nodeBefore->prev = newNode;
	
	return newNode;
}

template<class T>
void CNodeList<T>::InsertAfter(TNode* nodeAfter, TNode* fromNode, TNode* toNode)
{
	Detach( fromNode, toNode );

	toNode->next = nodeAfter->next;
	fromNode->prev = nodeAfter;
	if( nodeAfter == last ) {
		last = toNode;
	} else {
		nodeAfter->next->prev = toNode;
	}
	nodeAfter->next = fromNode;
}

template<class T>
void CNodeList<T>::Detach(TNode* fromNode, TNode* toNode)
{
	if( fromNode->prev != 0 ) {
		fromNode->prev->next = toNode->next;
	}
	if( toNode->next != 0 ) {
		toNode->next->prev = fromNode->prev;
	}
	if( fromNode == first ) {
		first = toNode->next;
	}
	if( toNode == last ) {
		last = fromNode->prev;
	}
	fromNode->prev = 0;
	toNode->next = 0;
}

template<class T>
void CNodeList<T>::Remove(TNode* fromNode, TNode* toNode)
{
	Detach( fromNode, toNode );
	
	while( fromNode != toNode ) {
		TNode* tmp = fromNode;
		fromNode = fromNode->next;
		free( tmp );
	}
	free( fromNode );
}

template<class T>
void CNodeList<T>::Copy(TNode* nodeAfter, TNode* fromNode, TNode* toNode)
{
	TNode* fromNodeCopy = 0;
	TNode* toNodeCopy = 0;
	Duplicate( fromNode, toNode, &fromNodeCopy, &toNodeCopy );
	InsertAfter( nodeAfter, fromNodeCopy, toNodeCopy );
}

template<class T>
void CNodeList<T>::Duplicate(const TNode* fromNode, const TNode* toNode,
	TNode** fromNodeCopy, TNode** toNodeCopy)
{
	TNode* tmpFirst = alloc( *fromNode );
	TNode* tmpLast = tmpFirst;
	
	while( fromNode != toNode ) {
		fromNode = fromNode->next;
		
		tmpLast->next = alloc( *fromNode );
		tmpLast->next->prev = tmpLast;
		tmpLast = tmpLast->next;
	}
	
	*fromNodeCopy = tmpFirst;
	*toNodeCopy = tmpLast;
}

template<class T>
typename CNodeList<T>::TNode* CNodeList<T>::Append(const T& value)
{
	if( last != 0 ) {
		InsertAfter( last, value );
	} else {
		first = alloc( value );
		last = first;
	}
	return last;
}

template<class T>
typename CNodeList<T>::TNode* CNodeList<T>::alloc(const T& value) 
{
	return new TNode( value );
}

template<class T>
void CNodeList<T>::free(TNode* node)
{
	delete node;
}

} // end of namespace Refal2
