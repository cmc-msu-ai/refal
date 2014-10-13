#include "Qualifier.h"

#include <iostream>

namespace Refal2 {

void CQualifier::print() const
{
	if( IsIncludeAllChars() ) {
		std::cout << "O \\ ";
	}
	print_set(static_cast<std::set<TChar> >(chars));
	std::cout << "\n";

	if( IsIncludeAllLabels() ) {
		std::cout << "F \\";
	}
	std::set<TLabel> a;
	labels.GetSet(a);
	for( std::set<TLabel>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		std::cout << " /" << (*i)->first << "/";
	}
	std::cout << "\n";

	if( IsIncludeAllNumbers() ) {
		std::cout << "N \\ ";
	}
	print_set(static_cast<std::set<TNumber> >(numbers));
	std::cout << "\n";

	if( IsIncludeTerms() ) {
		std::cout << "B";
	}
	std::cout << "\n\n";
}

bool CQualifier::Check(const CUnitLink* unit) const
{
	switch( unit->Type() ) {
		case CUnitLink::T_char:
			{
				TChar c = unit->Char();
				if( c >= 0 && static_cast<int>(c) < D_ansi_set_size ) {
					return ansichars.test(c);
				} else {
					return ( (chars.Find(c) == 0) != IsIncludeAllChars() );
				}
			}
			break;
		case CUnitLink::T_label:
			return ( (labels.Find(unit->Label()) == 0)
				!= IsIncludeAllLabels() );
			break;
		case CUnitLink::T_number:
			return ( (numbers.Find(unit->Number()) == 0)
				!= IsIncludeAllNumbers() );
			break;
		case CUnitLink::T_left_paren:
		case CUnitLink::T_right_paren:
			return IsIncludeTerms();
			break;
		default:
			/* assert */
			break;
	}
	/* assert */
	return false;
}

CQualifier& CQualifier::operator*=(CQualifier& q)
{
#define INTERSECT(FLAG, SET1, SET2) \
	if( (flags & FLAG) != 0 ) {\
		if( (q.flags & FLAG) != 0 ) {\
			SET1 *= SET2;\
		} else {\
			SET1 -= SET2;\
		}\
	} else {\
		if( (q.flags & FLAG) != 0 ) {\
			SET1.Swap(SET2);\
			flags |= FLAG;\
			SET1 -= SET2;\
		} else {\
			SET1 *= SET2;/* TODO: check */\
		}\
	}

	INTERSECT(F_all_chars, chars, q.chars);
	INTERSECT(F_all_labels, labels, q.labels);
	INTERSECT(F_all_numbers, numbers, q.numbers);
#undef INTERSECT

	if( ((flags & q.flags) & F_terms) == 0 ) {
		flags &= ~F_terms;
	}
	ansichars &= q.ansichars;
	return *this;
}

} // end of namespace refal2

