#pragma once

#include <set>
#include <bitset>
#include <Refal2.h>

namespace Refal2 {

void PrintQualifier(const CQualifier& qualifier);

const int AnsiSetSize = 128;
typedef std::bitset<AnsiSetSize> TAnsiSet;

enum TQualifierIncludeFlags {
	QIF_Terms = 0x1,
	QIF_AllChars = 0x2,
	QIF_AllLabels = 0x4,
	QIF_AllNumbers = 0x8
};

class CQualifier {
	friend class CQualifierBuilder;
	friend void PrintQualifier(const CQualifier& qualifier);

public:
	CQualifier(): flags(0) {}

	void Empty();
	void Move(CQualifier* toQualifier);

	void DestructiveIntersection(CQualifier* withQualifier);
	bool Check(const CUnit* unit) const;

	int GetIncludeFlags() const { return flags; }
	inline bool IsIncludeAllChars() const;
	inline bool IsIncludeAllLabels() const;
	inline bool IsIncludeAllNumbers() const;
	inline bool IsIncludeTerms() const;

private:
	int flags;
	TAnsiSet ansichars;
	CFastSet<TChar> chars;
	CFastSet<TLabel> labels;
	CFastSet<TNumber> numbers;
};

inline bool CQualifier::IsIncludeAllChars() const
{
	return ( (flags & QIF_AllChars) != 0 );
}

inline bool CQualifier::IsIncludeAllLabels() const
{
	return ( (flags & QIF_AllLabels) != 0 );
}

inline bool CQualifier::IsIncludeAllNumbers() const
{
	return ( (flags & QIF_AllNumbers) != 0 );
}

inline bool CQualifier::IsIncludeTerms() const
{
	return ( (flags & QIF_Terms) != 0 );
}

} // end of namespace refal2

