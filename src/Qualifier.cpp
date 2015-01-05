#include <set>
#include <bitset>
#include <iostream>
#include <Refal2.h>

namespace Refal2 {

template<class T>
static void PrintSet(const std::set<T>& set)
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

void PrintQualifier(const CQualifier& qualifier)
{
	if( qualifier.IsIncludeAllChars() ) {
		std::cout << "O \\ ";
	}
	PrintSet(static_cast<std::set<TChar> >(qualifier.chars));
	std::cout << "\n";

	if( qualifier.IsIncludeAllLabels() ) {
		std::cout << "F \\";
	}
	std::set<TLabel> a;
	qualifier.labels.GetSet(a);
	for( std::set<TLabel>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		std::cout << " /" << LabelTable.GetLabelText(*i) << "/";
	}
	std::cout << "\n";

	if( qualifier.IsIncludeAllNumbers() ) {
		std::cout << "N \\ ";
	}
	PrintSet(static_cast<std::set<TNumber> >(qualifier.numbers));
	std::cout << "\n";

	if( qualifier.IsIncludeTerms() ) {
		std::cout << "B";
	}
	std::cout << "\n\n";
}

void CQualifier::Empty()
{
	flags = 0;
	ansichars.reset();
	chars.Empty();
	labels.Empty();
	numbers.Empty();
}

void CQualifier::Move(CQualifier* toQualifier)
{
	toQualifier->flags = flags;
	toQualifier->ansichars = ansichars;
	toQualifier->chars.Swap( &chars );
	toQualifier->labels.Swap( &labels );
	toQualifier->numbers.Swap( &numbers );
	Empty();
}

bool CQualifier::Check(const CUnit* unit) const
{
	switch( unit->GetType() ) {
		case UT_Char:
			{
				TChar c = unit->Char();
				if( c >= 0 && static_cast<int>(c) < AnsiSetSize ) {
					return ansichars.test(c);
				} else {
					return ( (chars.Find(c) == 0) != IsIncludeAllChars() );
				}
			}
			break;
		case UT_Label:
			return ( (labels.Find(unit->Label()) == 0)
				!= IsIncludeAllLabels() );
			break;
		case UT_Number:
			return ( (numbers.Find(unit->Number()) == 0)
				!= IsIncludeAllNumbers() );
			break;
		case UT_LeftParen:
		case UT_RightParen:
			return IsIncludeTerms();
			break;
		default:
			assert( false );
			break;
	}
	assert( false );
	return false;
}

void CQualifier::DestructiveIntersection(CQualifier* withQualifier)
{
#define INTERSECT(FLAG, SET1, SET2) \
	if( (flags & FLAG) != 0 ) {\
		if( (withQualifier->flags & FLAG) != 0 ) {\
			SET1 *= SET2;\
		} else {\
			SET1 -= SET2;\
		}\
	} else {\
		if( (withQualifier->flags & FLAG) != 0 ) {\
			SET1.Swap( &SET2 );\
			flags |= FLAG;\
			SET1 -= SET2;\
		} else {\
			SET1 *= SET2;/* TODO: check */\
		}\
	}

	INTERSECT(QIF_AllChars, chars, withQualifier->chars);
	INTERSECT(QIF_AllLabels, labels, withQualifier->labels);
	INTERSECT(QIF_AllNumbers, numbers, withQualifier->numbers);
#undef INTERSECT

	if( ((flags & withQualifier->flags) & QIF_Terms) == 0 ) {
		flags &= ~QIF_Terms;
	}
	ansichars &= withQualifier->ansichars;
}

} // end of namespace refal2

