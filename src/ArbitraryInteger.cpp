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
	assert( digit < Base );
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

// [ - | x ] + [ - | y ] = [ - | x + y ]
// [ - | x ] + [ + | y ] = [ x < y ? + : - | x - y ]
// [ + | x ] + [ - | y ] = [ x < y ? - : + | x - y ]
// [ + | x ] + [ + | y ] = [ + | x + y ]

void CArbitraryInteger::Add( const CArbitraryInteger& operand )
{
	if( IsNegative() == operand.IsNegative() ) {
		// result sign will be the same of first operand
		add( operand );
	} else {
		sub( operand );
	}
	removeLeadingZeros();
}

// [ - | x ] - [ - | y ] = [ x < y ? + : - | x - y ]
// [ - | x ] - [ + | y ] = [ - | x + y ]
// [ + | x ] - [ - | y ] = [ + | x + y ]
// [ + | x ] - [ + | y ] = [ x < y ? - : + | x - y ]

void CArbitraryInteger::Sub( const CArbitraryInteger& operand )
{
	if( IsNegative() == operand.IsNegative() ) {
		sub( operand );
	} else {
		// result sign will be the same of first operand
		add( operand );
	}
	removeLeadingZeros();
}

void CArbitraryInteger::Mul( const CArbitraryInteger& operand )
{
	SetSign( IsNegative() != operand.IsNegative() );
	mul( operand );
	removeLeadingZeros();
}

void CArbitraryInteger::Div( CArbitraryInteger& operand )
{
	SetSign( IsNegative() != operand.IsNegative() );
	div( operand );
	removeLeadingZeros();
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
	add( begin(), operand );
}

void CArbitraryInteger::add( iterator i, const CArbitraryInteger& operand )
{
	const_iterator j = operand.cbegin();
	bool carryFlag = false;
	while( j != operand.cend() ) {
		assert( i != end() );
		*i += *j + ( carryFlag ? 1 : 0 );
		carryFlag = ( *i > Base );
		*i %= Base;
		++j;
		++i;
	}
	while( carryFlag ) {
		assert( i != end() );
		( *i )++;
		carryFlag = ( *i > Base );
		*i %= Base;
		++i;
	}
}

void CArbitraryInteger::sub( const CArbitraryInteger& operand )
{
	if( Compare( operand ) == CR_Less ) {
		CArbitraryInteger tmp;
		operand.Copy( tmp );
		Swap( tmp );
		isNegative = !isNegative;
		subFromBigger( tmp );
	} else {
		// result sign will be the same of first operand
		subFromBigger( operand );
	}
}

void CArbitraryInteger::subFromBigger( const CArbitraryInteger& operand )
{
	resize( std::max( size(), operand.size() ), 0 );
	iterator i = begin();
	const_iterator j = operand.cbegin();
	bool takeFlag = false;
	while( j != operand.cend() ) {
		assert( i != end() );
		if( takeFlag ) {
			( *i )--;
		}
		if( *i < *j ) {
			*i += Base - *j;
			takeFlag = true;
		} else {
			*i -= *j;
		}
		++j;
		++i;
	}
	while( takeFlag ) {
		assert( i != end() );
		if( *i > 0 ) {
			( *i )--;
			takeFlag = false;
		}
		++i;
	}
}

void CArbitraryInteger::mulDigitDigit( TDigit x, TDigit y,
	CArbitraryInteger& result ) const
{
	result.Zero();
#ifdef UINT64_MAX
	uint64_t xy = x * y;
	result.AddDigit( static_cast<TDigit>( xy % Base ) );
	result.AddDigit( static_cast<TDigit>( xy / Base ) );
#else
	static const TDigit base2 = sqrt( static_cast<double>( Base ) );
	const TDigit highX = x / base2;
	const TDigit lowX = x % base2;
	const TDigit highY = y / base2;
	const TDigit lowY = y % base2;
	const TDigit lowYlowX = lowY * lowX;
	const TDigit lowYhighX = lowY * highX;
	const TDigit highYlowX = highY * lowX;
	const TDigit highYhighX = highY * highX;
	const TDigit lowYhighXhighYlowX = lowYhighX + highYlowX;
	TDigit low = lowYlowX + ( lowYhighXhighYlowX % base2 ) * base2;
	TDigit high = highYhighX + ( lowYhighXhighYlowX / base2 ) + ( low / Base );
	low %= Base;
	result.AddDigit( low );
	result.AddDigit( high );
#endif
}

void CArbitraryInteger::mul( const CArbitraryInteger& operand )
{
	std::vector<value_type> tmp( cbegin(), cend() );
	clear();
	resize( tmp.size() + operand.size(), 0 );
	iterator firstDigit = begin();
	for( const_iterator i = tmp.cbegin(); i != tmp.cend(); ++i, ++firstDigit ) {
		iterator currentDigit = firstDigit;
		for( const_iterator j = operand.cbegin(); j != operand.cend(); ++j ) {
			CArbitraryInteger tmp;
			mulDigitDigit( *i, *j, tmp );
			add( currentDigit, tmp );
			++currentDigit;
		}
	}
}

void CArbitraryInteger::div( CArbitraryInteger& operand )
{
	// todo: implement
	Zero();
	operand.Zero();
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
