#pragma once

#include "FastSet.h"
#include "Common.h"
#include <bitset>
#include <set>

template<class T>
void print_set(const std::set<T>& set)
{
	std::cout << "{";
	typename std::set<T>::const_iterator i = set.begin();
	for( ; i != set.end(); )
	{
		std::cout << *i;
		++i;
		if( i != set.end() ) {
			std::cout << ", ";
		} else {
			break;
		}
	}
	std::cout << "}";
}

namespace Refal2 {

typedef std::bitset<D_ansi_set_size> TAnsiSet;

class CQualifier {
	friend class CQualifierBuilder;
public:
	CQualifier(): flags(0) {}

	CQualifier& operator*=(CQualifier&);
	void print() const;

	bool Check(const CUnitLink* unit) const;

	bool IsIncludeAllChars() const
	{
		return ( (flags & F_all_chars) != 0 );
	}
	bool IsIncludeAllLabels() const
	{
		return ( (flags & F_all_labels) != 0 );
	}
	bool IsIncludeAllNumbers() const
	{
		return ( (flags & F_all_numbers) != 0 );
	}
	bool IsIncludeTerms() const
	{
		return ( (flags & F_terms) != 0 );
	}
private:
	TAnsiSet ansichars;
	CFastSet<TChar> chars;
	CFastSet<TLabel> labels;
	CFastSet<TNumber> numbers;

	enum TFlag {
		F_terms = 0x1,
		F_all_chars = 0x2,
		F_all_labels = 0x4,
		F_all_numbers = 0x8
	};
	int flags;
};

} // end of namespace refal2

