#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CArbitraryInteger

class CArbitraryInteger : private std::vector<TNumber> {
public:
	typedef value_type TDigit;
	typedef size_type TDigitIndex;
	static const TDigit Base = 1 << 24;
	static_assert( sizeof( TDigit ) >= 4, "too small digit");

	CArbitraryInteger() { Zero(); }
	explicit CArbitraryInteger( TDigit digit );
	explicit CArbitraryInteger( const std::string& text );

	enum TCompareResult {
		CR_Less = -1,
		CR_Equal,
		CR_Great
	};

	void Zero();
	bool IsZero() const { return ( GetSize() == 0 ); }
	void SetSign( bool _isNegative = false ) { isNegative = _isNegative; }
	TDigitIndex GetSize() const;
	// add next digit of number
	// ex. Zero(); AddDigit( 301 ); AddDigit( 3 ); => 3 * 2^24 + 301
	void AddDigit( TDigit digit );
	// next function don't grow number
	void SetDigit( TDigitIndex pos, TDigit digit );
	TDigit GetDigit( TDigitIndex pos ) const;
	// ex. SetValueByText( "-12345" );
	//   means Zero(); SetSign( true ); AddDigit( 12345 );
	void SetValueByText( const std::string& text );
	// get number value as text
	void GetTextValue( std::string& text ) const;
	bool IsNegative() const { return isNegative; }
	// arithmetic functions
	void Add( const CArbitraryInteger& operand );
	void Sub( const CArbitraryInteger& operand );
	void Mul( const CArbitraryInteger& operand );
	// operand saves rest
	void Div( CArbitraryInteger& operand );
	TCompareResult Compare( const CArbitraryInteger& operand ) const;

private:
	bool isNegative;

	void removeLeadingZeros();
	void add( const CArbitraryInteger& operand );
	void sub( const CArbitraryInteger& operand );
	void mul( const CArbitraryInteger& operand );
	void div( CArbitraryInteger& operand );

	CArbitraryInteger( const CArbitraryInteger& );
	CArbitraryInteger& operator=( const CArbitraryInteger& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
