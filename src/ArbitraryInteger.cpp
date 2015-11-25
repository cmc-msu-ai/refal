#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CArbitraryInteger

CArbitraryInteger::CArbitraryInteger( TDigit digit )
{
	Zero();
	AddDigit( digit );
}

void CArbitraryInteger::Swap( CArbitraryInteger& swapWith )
{
	swapDigits( swapWith );
	std::swap( isNegative, swapWith.isNegative );
}

void CArbitraryInteger::Copy( CArbitraryInteger& copyTo ) const
{
	copyTo.assign( cbegin(), cend() );
	copyTo.isNegative = isNegative;
}

void CArbitraryInteger::Zero()
{
	clear();
	SetSign( false );
}

CArbitraryInteger::TDigitIndex CArbitraryInteger::GetSize() const
{
	size_type result = size();
	const_reverse_iterator i = crbegin();
	while( i != crend() && *i == 0 ) {
		++i;
		result--;
	}
	return result;
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
}

CArbitraryInteger::TDigit CArbitraryInteger::GetDigit( TDigitIndex pos ) const
{
	assert( pos < GetSize() );
	return operator[]( pos );
}

bool CArbitraryInteger::SetValueByText( const std::string& text )
{
	Zero();
	if( text.empty() ) {
		return true;
	}
	std::string::const_iterator c = text.cbegin();
	if( *c == '-' || *c == '+' ) {
		SetSign( *c == '-' );
		++c;
	}
	TDigit digit = 0;
	const int maxDecimalDigits = 7;
	const TDigit maxDecimal = 10000000;
	static_assert( Base > maxDecimal, "too small Base" );
	int decimalCount = 0;
	for( ; c != text.cend(); ++c ) {
		if( *c >= '0' && *c <= '9' ) {
			digit = digit * 10 + ( *c - '0' );
			decimalCount++;
			if( decimalCount == maxDecimalDigits ) {
				mul( CArbitraryInteger( maxDecimal ) );
				add( CArbitraryInteger( digit ) );
				digit = 0;
				decimalCount = 0;
			}
		} else {
			Zero();
			return false;
		}
	}
	TDigit decimal = 1;
	for( ; decimalCount > 0; decimalCount-- ) {
		decimal *= 10;
	}
	mul( CArbitraryInteger( decimal ) );
	add( CArbitraryInteger( digit ) );
	return true;
}

void CArbitraryInteger::GetTextValue( std::string& text ) const
{
	const_cast<CArbitraryInteger&>( *this ).removeLeadingZeros();
	if( IsZero() ) {
		text = "0";
		return;
	}
	text.clear();
	CArbitraryInteger tmp;
	Copy( tmp );
	while( !tmp.IsZero() ) {
		CArbitraryInteger decimal( 10000000 );
		tmp.Div( decimal );
		assert( decimal.size() == 1 );
		std::ostringstream stringStream;
		stringStream << std::setw( 7 ) << std::setfill( '0' ) << decimal[0];
		text = stringStream.str() + text;
	}
	text.erase( 0, text.find_first_not_of( '0' ) );
	if( IsNegative() ) {
		text = "-" + text;
	}
}

// [ - | x ] + [ - | y ] = [ - | x + y ]
// [ - | x ] + [ + | y ] = [ x < y ? + : - | x - y ]
// [ + | x ] + [ - | y ] = [ x < y ? - : + | x - y ]
// [ + | x ] + [ + | y ] = [ + | x + y ]

void CArbitraryInteger::Add( const CArbitraryInteger& operand )
{
	removeLeadingZeros();
	const_cast<CArbitraryInteger&>( operand ).removeLeadingZeros();
	if( IsNegative() == operand.IsNegative() ) {
		// result sign will be the same of operands
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
	removeLeadingZeros();
	const_cast<CArbitraryInteger&>( operand ).removeLeadingZeros();
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
	removeLeadingZeros();
	const_cast<CArbitraryInteger&>( operand ).removeLeadingZeros();
	const bool resultSign = IsNegative() != operand.IsNegative();
	mul( operand );
	SetSign( resultSign );
	removeLeadingZeros();
}

void CArbitraryInteger::Div( CArbitraryInteger& operand )
{
	removeLeadingZeros();
	operand.removeLeadingZeros();
	const bool operandSign = IsNegative();
	const bool resultSign = IsNegative() != operand.IsNegative();
	div( operand );
	SetSign( resultSign );
	operand.SetSign( operandSign );
	removeLeadingZeros();
}

CArbitraryInteger::TCompareResult CArbitraryInteger::Compare(
	const CArbitraryInteger& operand ) const
{
	const_cast<CArbitraryInteger&>( *this ).removeLeadingZeros();
	const_cast<CArbitraryInteger&>( operand ).removeLeadingZeros();
	if( IsNegative() == operand.IsNegative() ) {
		switch( compare( operand ) ) {
			case CR_Less:
				return ( IsNegative() ? CR_Great : CR_Less );
			case CR_Equal:
				return CR_Equal;
			case CR_Great:
				break;
			default:
				assert( false );
				break;
		}
	}
	return ( IsNegative() ? CR_Less : CR_Great );
}

void CArbitraryInteger::BitwiseShiftLeft( int bitsCount )
{
	if( empty() ) {
		return;
	}
	removeLeadingZeros();
	const int digitsToAdd = bitsCount / DegreeOfBase;
	const int shift = bitsCount % DegreeOfBase;
	insert( begin(), digitsToAdd, 0 );
	TDigit save = 0;
	for( iterator i = begin() + digitsToAdd; i != end(); ++i ) {
		TDigit tmp = ( *i ) >> ( DegreeOfBase - shift );
		*i = ( ( ( *i ) << shift ) | save ) % Base;
		save = tmp;
	}
	if( save > 0 ) {
		push_back( save );
	}
}

void CArbitraryInteger::BitwiseShiftRight( int bitsCount )
{
	removeLeadingZeros();
	assert( bitsCount > 0 );
	const size_type digitsToDelete =
		std::min<size_type>( bitsCount / DegreeOfBase, size() );
	const int shift = bitsCount % DegreeOfBase;
	erase( begin(), begin() + digitsToDelete );
	if( !empty() && shift > 0 ) {
		assert( shift < DegreeOfBase );
		TDigit save = 0;
		for( reverse_iterator i = rbegin(); i != rend(); ++i ) {
			TDigit tmp = ( ( *i ) << ( DegreeOfBase - shift ) ) % Base;
			*i = ( ( *i ) >> shift ) | save;
			save = tmp;
		}
	}
	removeLeadingZeros();
}

void CArbitraryInteger::swapDigits( CArbitraryInteger& number )
{
	number.std::vector<uint32_t>::swap( *this );
}

void CArbitraryInteger::removeLeadingZeros()
{
	while( !empty() && back() == 0 ) {
		erase( begin() + ( size() - 1 ) );
	}
}

CArbitraryInteger::TCompareResult CArbitraryInteger::compare(
	const CArbitraryInteger& operand ) const
{
	if( size() < operand.size() ) {
		return CR_Less;
	} else if( operand.size() < size() ) {
		return CR_Great;
	} else {
		assert( operand.size() == size() );
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
	switch( compare( operand ) ) {
		case CR_Equal:
			Zero();
			break;
		case CR_Less:
		{
			CArbitraryInteger tmp;
			operand.Copy( tmp );
			// result sign will be the negative of first operand
			swapDigits( tmp );
			isNegative = !isNegative;
			subFromBigger( tmp );
			break;
		}
		case CR_Great:
			// result sign will be the same of first operand
			subFromBigger( operand );
			break;
		default:
			assert( false );
			break;
	}
}

void CArbitraryInteger::subFromBigger( const CArbitraryInteger& operand )
{
	resize( std::max( size(), operand.size() ), 0 );
	iterator i = begin();
	const_iterator j = operand.cbegin();
	bool take = false;
	while( j != operand.cend() ) {
		assert( i != end() );
		TDigit sub = *j + ( take ? 1 : 0 );
		if( *i < sub ) {
			*i += Base - sub;
			take = true;
		} else {
			*i -= sub;
			take = false;
		}
		++j;
		++i;
	}
	while( take ) {
		assert( i != end() );
		if( *i > 0 ) {
			( *i )--;
			take = false;
		}
		++i;
	}
}

void CArbitraryInteger::mulDigitDigit( TDigit x, TDigit y,
	CArbitraryInteger& result ) const
{
	result.Zero();
#ifdef UINT64_MAX
	uint64_t xy = static_cast<uint64_t>( x ) * static_cast<uint64_t>( y );
	result.AddDigit( static_cast<TDigit>( xy % Base ) );
	result.AddDigit( static_cast<TDigit>( xy / Base ) );
#else
	const TDigit baseRoot = 1 << ( DegreeOfBase / 2 ); // 2^12 = 4096
	static_assert( baseRoot * baseRoot == Base, "wrong Base" );
	const TDigit highX = x / baseRoot;
	const TDigit lowX = x % baseRoot;
	const TDigit highY = y / baseRoot;
	const TDigit lowY = y % baseRoot;
	const TDigit lowYlowX = lowY * lowX;
	const TDigit lowYhighX = lowY * highX;
	const TDigit highYlowX = highY * lowX;
	const TDigit highYhighX = highY * highX;
	const TDigit mid = lowYhighX + highYlowX;
	TDigit low = lowYlowX + ( mid % baseRoot ) * baseRoot;
	TDigit high = highYhighX + ( mid / baseRoot ) + ( low / Base );
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
	assert( !operand.IsZero() );
	if( IsZero() ) {
		// 0 / ...
		operand.Zero();
	} else {
		switch( compare( operand ) ) {
			case CR_Less:
				// x / y, where x < y
				swap( operand );
				Zero();
				break;
			case CR_Equal:
				Zero();
				AddDigit( 1 );
				operand.Zero();
				break;
			case CR_Great:
			{
				// x > y > 0
				CArbitraryInteger result;
				calculateDiv( operand, result );
				swap( operand );
				swap( result );
				break;
			}
			default:
				assert( false );
				break;
		}
	}
}

void CArbitraryInteger::calculateDiv( CArbitraryInteger& operand,
	CArbitraryInteger& result )
{
	int shift = divCalculateShift( operand );
	operand.BitwiseShiftLeft( shift );
	result.clear();
	result.resize( shift / DegreeOfBase + 1, 0 );
	reverse_iterator i = result.rbegin();
	TDigit mask = 1 << ( shift % DegreeOfBase );
	for( ; shift >= 0; shift-- ) {
		switch( compare( operand ) ) {
			case CR_Less:
				break;
			case CR_Equal:
			case CR_Great:
				( *i ) |= mask;
				subFromBigger( operand );
				removeLeadingZeros();
				break;
			default:
				assert( false );
				break;
		}
		operand.BitwiseShiftRight( 1 );
		mask >>= 1;
		if( mask == 0 ) {
			++i;
			mask = Base >> 1;
		}
	}
}

int CArbitraryInteger::divCalculateShift( const CArbitraryInteger& operand )
{
	TDigit a = back();
	TDigit b = operand.back();
	int shift = ( size() - operand.size() ) * DegreeOfBase;
	if( a < b ) {
		do {
			b >>= 1;
			shift--;
		} while( a <= b );
		shift++;
	} else if( b < a ) {
		do {
			b <<= 1;
			shift++;
		} while( b <= a );
		shift--;
	}
	return shift;
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
