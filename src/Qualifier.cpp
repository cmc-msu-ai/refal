#include <set>
#include <bitset>
#include <iostream>
#include <Refal2.h>

namespace Refal2 {

static void printAnsiSet(const TAnsiSet& ansichars, const std::string& chars,
	const std::string& mark)
{
	bool isInclude = false;
	if( ansichars.count() > chars.length() / 2 ) {
		isInclude = true;
	}

	if( isInclude ) {
		std::cout << "(";
	}

	std::cout << "'";
	for( std::size_t i = 0; i < chars.length(); i++ ) {
		if( ansichars.test( chars[i] ) != isInclude ) {
			std::cout << chars[i];
		}
	}
	std::cout << "'";

	if( isInclude ) {
		std::cout << ")" << mark << " ";
	} else {
		std::cout << "(" << mark << ") ";
	}
}

static void printChars(const CFastSet<TChar>& chars, const TAnsiSet& ansichars,
	bool isIncludeAll)
{
	TAnsiSet L = ansichars & CQualifierBuilder::AnsiL;
	TAnsiSet D = ansichars & CQualifierBuilder::AnsiD;

	printAnsiSet( L, CQualifierBuilder::Alphabet, "L" );
	printAnsiSet( D, CQualifierBuilder::Numbers, "D" );

	TAnsiSet other = ansichars & ~(L | D);
	bool superFlag = other.count() < 31;
	if( !superFlag ) {
		other |= CQualifierBuilder::AnsiL | CQualifierBuilder::AnsiD;
		std::cout << "(";
	}
	std::cout << "'";	
	for( int i = 1; i < AnsiSetSize; i++ ) {
		if( other.test( i ) == superFlag ) {
			std::cout << static_cast<char>( i );
		}
	}
	std::cout << "' ";
	if( !superFlag ) {
		std::cout << ")";
	}

	if( isIncludeAll && !chars.IsEmpty() ) {
		std::cout << "(";
	}
	if( !chars.IsEmpty() ) {
		std::cout << "'";
	}
	std::set<TChar> a;
	chars.GetSet( &a );
	for( std::set<TChar>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		std::cout << *i;
	}
	if( !chars.IsEmpty() ) {
		std::cout << "'";
	}
	if( isIncludeAll && !chars.IsEmpty() ) {
		std::cout << ") ";
	}

	if( isIncludeAll ) {
		std::cout << "O ";
	} else {
		std::cout << "(O) ";
	} 
}

static void printLabels(const CFastSet<TLabel>& labels, bool isIncludeAll)
{
	if( isIncludeAll && !labels.IsEmpty() ) {
		std::cout << "(";
	}

	std::set<TLabel> a;
	labels.GetSet( &a );
	for( std::set<TLabel>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		std::cout << "/" << LabelTable.GetLabelText(*i) << "/";
	}

	if( isIncludeAll && !labels.IsEmpty() ) {
		std::cout << ")";
	}

	if( isIncludeAll ) {
		std::cout << "F ";
	} else {
		std::cout << "(F) ";
	}
}

static void printNumbers(const CFastSet<TNumber>& numbers, bool isIncludeAll)
{
	if( isIncludeAll && !numbers.IsEmpty() ) {
		std::cout << "(";
	}

	std::set<TNumber> a;
	numbers.GetSet( &a );
	for( std::set<TNumber>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		std::cout << "/" << *i << "/";
	}

	if( isIncludeAll && !numbers.IsEmpty() ) {
		std::cout << ")";
	}

	if( isIncludeAll ) {
		std::cout << "N ";
	} else {
		std::cout << "(N) ";
	}
}

void PrintQualifier(const CQualifier& qualifier)
{
	if( !qualifier.IsEmpty() ) {
		std::cout << "( ";

		printChars( qualifier.chars, qualifier.ansichars,
			qualifier.IsIncludeAllChars() );
		printLabels( qualifier.labels, qualifier.IsIncludeAllLabels() );
		printNumbers( qualifier.numbers, qualifier.IsIncludeAllNumbers() );

		if( qualifier.IsIncludeTerms() ) {
			std::cout << "B ";
		} else {
			std::cout << "(B) ";
		}

		std::cout << ")";
	}
}

void CQualifier::Empty()
{
	flags = QIF_All;
	ansichars.set();
	chars.Empty();
	labels.Empty();
	numbers.Empty();
}

bool CQualifier::IsEmpty() const
{
	return ( ( ( flags & QIF_All ) == QIF_All ) && ansichars.all() &&
		chars.IsEmpty() && labels.IsEmpty() && numbers.IsEmpty() );
}

void CQualifier::Swap(CQualifier* swapWith)
{
	std::swap( flags, swapWith->flags );
	std::swap( ansichars, swapWith->ansichars );
	swapWith->chars.Swap( &chars );
	swapWith->labels.Swap( &labels );
	swapWith->numbers.Swap( &numbers );
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
				return ( chars.Has(c) != IsIncludeAllChars() );
			}
			break;
		}
		case UT_Label:
			return ( labels.Has( unit->Label() ) != IsIncludeAllLabels() );
			break;
		case UT_Number:
			return ( numbers.Has( unit->Number() ) != IsIncludeAllNumbers() );
			break;
		case UT_LeftParen:
		case UT_RightParen:
			return IsIncludeTerms();
			break;
		default:
			break;
	}
	assert( false );
	return false;
}

void CQualifier::DestructiveIntersection(CQualifier* withQualifier)
{
	if( IsEmpty() ) {
		Swap( withQualifier );
	} else {

		ansichars &= withQualifier->ansichars;

		if( !withQualifier->IsIncludeTerms() ) {
			flags &= ~QIF_Terms;
		}
	
#define INTERSECT(FLAG, SET)\
		if( ( flags & FLAG ) != 0 ) {\
			if( ( withQualifier->flags & FLAG ) != 0 ) {\
				SET += withQualifier->SET;\
			} else {\
				flags &= ~FLAG;\
				withQualifier->SET.Swap( &SET );\
				SET -= withQualifier->SET;\
			}\
		} else {\
			if( ( withQualifier->flags & FLAG ) != 0 ) {\
				SET -= withQualifier->SET;\
			} else {\
				SET *= withQualifier->SET;\
			}\
		}

		INTERSECT( QIF_AllChars, chars );
		INTERSECT( QIF_AllLabels, labels );
		INTERSECT( QIF_AllNumbers, numbers );
#undef INTERSECT

		withQualifier->Empty();
	}
}

} // end of namespace refal2
