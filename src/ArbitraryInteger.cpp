#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CArbitraryInteger

CArbitraryInteger::CArbitraryInteger( TDigit digit )
{
	Zero();
	AddDigit( digit );
}

CArbitraryInteger::CArbitraryInteger( const std::string& text )
{
	SetValueByText( text );
}

void CArbitraryInteger::Swap( CArbitraryInteger& swapWith )
{
	this->swap( swapWith );
	std::swap( isNegative, swapWith.isNegative );
}

void CArbitraryInteger::Copy( CArbitraryInteger& copyTo ) const
{
	copyTo.assign( cbegin(), cend() );
	copyTo.isNegative = isNegative;
}

void CArbitraryInteger::Zero()
{
	SetSign( false );
	clear();
}

CArbitraryInteger::TDigitIndex CArbitraryInteger::GetSize() const
{
	return size();
}

void CArbitraryInteger::AddDigit( TDigit digit )
{
	assert( digit < Base );
	push_back( digit );
}

void CArbitraryInteger::SetDigit( TDigitIndex pos, TDigit digit )
{
	assert( pos < GetSize() );
	operator[]( pos ) = digit;
	removeLeadingZeros();
}

CArbitraryInteger::TDigit CArbitraryInteger::GetDigit( TDigitIndex pos ) const
{
	assert( pos < GetSize() );
	return operator[]( pos );
}

void CArbitraryInteger::SetValueByText( const std::string& text )
{
	// todo: implement
	Zero();
}

void CArbitraryInteger::GetTextValue( std::string& text ) const
{
	// todo: implement
	text.clear();
}

void CArbitraryInteger::Add( const CArbitraryInteger& operand )
{
	if( IsNegative() == operand.IsNegative() ) {
		add( operand );
	} else {
		sub( operand );
	}
}

void CArbitraryInteger::Sub( const CArbitraryInteger& operand )
{
	if( IsNegative() != operand.IsNegative() ) {
		add( operand );
	} else {
		sub( operand );
	}
}

void CArbitraryInteger::Mul( const CArbitraryInteger& operand )
{
	SetSign( IsNegative() != operand.IsNegative() );
	mul( operand );
}

void CArbitraryInteger::Div( CArbitraryInteger& operand )
{
	SetSign( IsNegative() != operand.IsNegative() );
	div( operand );
}

CArbitraryInteger::TCompareResult CArbitraryInteger::Compare(
	const CArbitraryInteger& operand ) const
{
	if( GetSize() < operand.GetSize() ) {
		return CR_Less;
	} else if( operand.GetSize() < GetSize() ) {
		return CR_Great;
	} else {
		assert( operand.GetSize() == GetSize() );
		const_reverse_iterator i = crbegin();
		const_reverse_iterator j = operand.crbegin();
		while( i != crend() && *i == *j ) {
			++i;
			++j;
		}
		if( i == crend() ) {
			assert( j == operand.crend() );
			return CR_Equal;
		}
		assert( *i != *j );
		return ( *i < *j ? CR_Less : CR_Great );
	}
}

void CArbitraryInteger::removeLeadingZeros()
{
	while( !empty() && back() == 0 ) {
		erase( begin() + ( size() - 1 ) );
	}
}

void CArbitraryInteger::add( const CArbitraryInteger& operand )
{
	resize( std::max( size(), operand.size() ) + 1, 0 );
	iterator i = begin();
	const_iterator j = operand.cbegin();
	TDigit savedDigit = 0;
	while( j != operand.cend() ) {
		assert( i != end() );
		*i += *j + savedDigit;
		savedDigit = *i > Base ? 1 : 0;
		++j;
		++i;
	}
	removeLeadingZeros();
}

void CArbitraryInteger::sub( const CArbitraryInteger& operand )
{
	// todo: implement
	Zero();
}

void CArbitraryInteger::mul( const CArbitraryInteger& operand )
{
	// todo: implement
	Zero();
}

void CArbitraryInteger::div( CArbitraryInteger& operand )
{
	// todo: implement
	Zero();
	operand.Zero();
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
