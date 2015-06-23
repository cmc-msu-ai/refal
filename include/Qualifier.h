#pragma once

#include <Refal2.h>
#include <set>
#include <bitset>

namespace Refal2 {

const std::size_t AnsiSetSize = 128;
typedef std::bitset<AnsiSetSize> CAnsiSet;

enum TQualifierIncludeFlags {
	QIF_Terms = 0x1,
	QIF_AllChars = 0x2,
	QIF_AllLabels = 0x4,
	QIF_AllNumbers = 0x8,
	QIF_All = QIF_Terms | QIF_AllChars | QIF_AllLabels | QIF_AllNumbers
};

void PrintQualifier( const CQualifier& qualifier );

class CQualifier {
	friend class CQualifierBuilder;
	friend void PrintQualifier( const CQualifier& qualifier );

public:
	CQualifier(): flags( 0 ) {}

	void Empty();
	bool IsEmpty() const;

	void Swap( CQualifier& swapWith );
	void Move( CQualifier& moveTo );

	void DestructiveIntersection( CQualifier& withQualifier );
	bool Check( const CUnit& unit ) const;

	int GetIncludeFlags() const { return flags; }
	bool IsIncludeAllChars() const;
	bool IsIncludeAllLabels() const;
	bool IsIncludeAllNumbers() const;
	bool IsIncludeTerms() const;

private:
	int flags;
	CAnsiSet ansichars;
	CFastSet<TChar> chars;
	CFastSet<TLabel> labels;
	CFastSet<TNumber> numbers;
};

inline void CQualifier::Move( CQualifier& moveTo )
{
	if( this != &moveTo ) {
		Swap( moveTo );
		Empty();
	}
}

inline bool CQualifier::IsIncludeAllChars() const
{
	return ( ( flags & QIF_AllChars ) != 0 );
}

inline bool CQualifier::IsIncludeAllLabels() const
{
	return ( ( flags & QIF_AllLabels ) != 0 );
}

inline bool CQualifier::IsIncludeAllNumbers() const
{
	return ( ( flags & QIF_AllNumbers ) != 0 );
}

inline bool CQualifier::IsIncludeTerms() const
{
	return ( ( flags & QIF_Terms ) != 0 );
}

} // end of namespace refal2

