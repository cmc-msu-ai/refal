#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CReceptacle

const char Equal = '=';

bool CReceptacle::Burry( CUnitList& keyValue )
{
	if( !IsValidKeyValue( keyValue ) ) {
		return false;
	}
	burry( keyValue );
	assert( keyValue.IsEmpty() );
	return true;
}

bool CReceptacle::Replace( CUnitList& keyValue )
{
	if( !IsValidKeyValue( keyValue ) ) {
		return false;
	}
	CUnitNode* leftBorderNode = nullptr;
	CUnitNode* equalNode = nullptr;
	if( find( keyValue.GetFirst(), leftBorderNode, equalNode ) ) {
		data.Remove( leftBorderNode->Next(),
			leftBorderNode->PairedParen()->Prev() );
		data.InsertAfter( leftBorderNode, keyValue );
	} else {
		burry( keyValue );
	}
	assert( keyValue.IsEmpty() );
	return true;
}

void CReceptacle::DigOut( const CUnitList& key, CUnitList& value )
{
	CUnitNode* leftBorderNode = nullptr;
	CUnitNode* equalNode = nullptr;
	if( find( key.GetFirst(), leftBorderNode, equalNode ) ) {
		CUnitNode* rightBorderNode = leftBorderNode->PairedParen();
		equalNode = equalNode->Next();
		if( equalNode != rightBorderNode ) {
			rightBorderNode = rightBorderNode->Prev();
			data.Detach( equalNode, rightBorderNode );
			value.Assign( equalNode, rightBorderNode );
			data.Remove( leftBorderNode, leftBorderNode->PairedParen() );
			return;
		}
	}
	value.Empty();	
}

void CReceptacle::Copy( const CUnitList& key, CUnitList& value )
{
	CUnitNode* leftBorderNode = nullptr;
	CUnitNode* equalNode = nullptr;
	if( find( key.GetFirst(), leftBorderNode, equalNode ) ) {
		CUnitNode* rightBorderNode = leftBorderNode->PairedParen();
		equalNode = equalNode->Next();
		if( equalNode != rightBorderNode ) {
			rightBorderNode = rightBorderNode->Prev();
			CUnitNode* first = nullptr;
			CUnitNode* last = nullptr;
			data.Duplicate( equalNode, rightBorderNode, first, last );
			value.Assign( first, last );
			return;
		}
	}
	value.Empty();
}

void CReceptacle::DigOutAll( CUnitList& receptacle )
{
	data.Move( receptacle );
}

static bool isEqualNode( const CUnitNode* node )
{
	return ( node->IsChar() && node->Char() == Equal );
}

bool CReceptacle::IsValidKeyValue( const CUnitList& keyValue )
{
	int depth = 0;
	const CUnitNode* node = keyValue.GetFirst();
	while( node != nullptr ) {
		if( depth == 0 && isEqualNode( node ) ) {
			return true;
		}
		if( node->IsLeftParen() ) {
			depth++;
		} else if( node->IsRightParen() ) {
			assert( depth > 0 );
			depth--;
		}
		node = node->Next();
	}
	return false;
}

void CReceptacle::burry( CUnitList& keyValue )
{
	// add ( keyValue ) to the beginning of the data
	CUnitList tmp;
	tmp.InsertAfter( tmp.AppendParens(), keyValue );
	tmp.Append( data );
	tmp.Move( data );
}

bool CReceptacle::find( const CUnitNode* key,
	CUnitNode*& leftBorderNode, CUnitNode*& equalNode )
{
	leftBorderNode = data.GetFirst();
	while( leftBorderNode != nullptr ) {
		assert( leftBorderNode->IsLeftParen() );
		const CUnitNode* keyNode = key;
		CUnitNode* burriedKeyNode = leftBorderNode->Next();
		int depth = 0;
		while( keyNode != nullptr && keyNode->IsEqualWith( *burriedKeyNode )
			&& ( depth > 0 || !isEqualNode( keyNode ) ) )
		{
			if( keyNode->IsLeftParen() ) {
				depth++;
			} else if( keyNode->IsRightParen() ) {
				assert( depth > 0 );
				depth--;
			}
			keyNode = keyNode->Next();
			burriedKeyNode = burriedKeyNode->Next();
		}
		if( ( keyNode == nullptr || isEqualNode( keyNode ) )
			&& isEqualNode( burriedKeyNode ) )
		{
			assert( depth == 0 );
			equalNode = burriedKeyNode;
			return true;
		}
		leftBorderNode = leftBorderNode->PairedParen()->Next();
	}
	equalNode = nullptr;
	return false;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
