#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

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
	return ( ( ( flags & QIF_All ) == QIF_All ) && ansichars.all()
		&& chars.IsEmpty() && labels.IsEmpty() && numbers.IsEmpty() );
}

void CQualifier::Swap( CQualifier& swapWith )
{
	std::swap( flags, swapWith.flags );
	std::swap( ansichars, swapWith.ansichars );
	swapWith.chars.Swap( &chars );
	swapWith.labels.Swap( &labels );
	swapWith.numbers.Swap( &numbers );
}

bool CQualifier::Check( const CUnit& unit ) const
{
	switch( unit.GetType() ) {
		case UT_Char:
		{
			TChar c = unit.Char();
			if( c >= 0 && static_cast<int>(c) < AnsiSetSize ) {
				return ansichars.test(c);
			} else {
				return ( chars.Has(c) != IsIncludeAllChars() );
			}
			break;
		}
		case UT_Label:
			return ( labels.Has( unit.Label() ) != IsIncludeAllLabels() );
			break;
		case UT_Number:
			return ( numbers.Has( unit.Number() ) != IsIncludeAllNumbers() );
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

void CQualifier::DestructiveIntersection( CQualifier& withQualifier )
{
	if( IsEmpty() ) {
		Swap( withQualifier );
	} else {

		ansichars &= withQualifier.ansichars;

		if( !withQualifier.IsIncludeTerms() ) {
			flags &= ~QIF_Terms;
		}
	
#define INTERSECT(FLAG, SET)\
		if( ( flags & FLAG ) != 0 ) {\
			if( ( withQualifier.flags & FLAG ) != 0 ) {\
				SET += withQualifier.SET;\
			} else {\
				flags &= ~FLAG;\
				withQualifier.SET.Swap( &SET );\
				SET -= withQualifier.SET;\
			}\
		} else {\
			if( ( withQualifier.flags & FLAG ) != 0 ) {\
				SET -= withQualifier.SET;\
			} else {\
				SET *= withQualifier.SET;\
			}\
		}

		INTERSECT( QIF_AllChars, chars );
		INTERSECT( QIF_AllLabels, labels );
		INTERSECT( QIF_AllNumbers, numbers );
#undef INTERSECT

		withQualifier.Empty();
	}
}

void CQualifier::Print( std::ostream& outputStream,
	const CPrintHelper& printHelper ) const
{
	if( IsEmpty() ) {
		return;
	}
	outputStream << "( ";
	printChars( outputStream );
	printLabels( outputStream, printHelper );
	printNumbers( outputStream );
	outputStream << ( IsIncludeTerms() ? "B" : "(B)" ) << " )";
}

static void printAnsiSet( std::ostream& outputStream, const CAnsiSet& ansichars,
	const std::string& chars, const std::string& mark )
{
	bool isInclude = false;
	if( ansichars.count() > chars.length() / 2 ) {
		isInclude = true;
	}

	if( isInclude ) {
		outputStream << "(";
	}

	outputStream << "'";
	for( std::size_t i = 0; i < chars.length(); i++ ) {
		if( ansichars.test( chars[i] ) != isInclude ) {
			outputStream << chars[i];
		}
	}
	outputStream << "'";

	if( isInclude ) {
		outputStream << ")" << mark << " ";
	} else {
		outputStream << "(" << mark << ") ";
	}
}

void CQualifier::printChars( std::ostream& outputStream ) const
{
	CAnsiSet L = ansichars & CQualifierBuilder::AnsiL;
	CAnsiSet D = ansichars & CQualifierBuilder::AnsiD;

	printAnsiSet( outputStream, L, CQualifierBuilder::Alphabet, "L" );
	printAnsiSet( outputStream, D, CQualifierBuilder::Numbers, "D" );

	CAnsiSet other = ansichars & ~(L | D);
	bool superFlag = other.count() < 31;
	if( !superFlag ) {
		other |= CQualifierBuilder::AnsiL | CQualifierBuilder::AnsiD;
		outputStream << "(";
	}
	outputStream << "'";
	for( int i = 1; i < AnsiSetSize; i++ ) {
		if( other.test( i ) == superFlag ) {
			outputStream << static_cast<char>( i );
		}
	}
	outputStream << "' ";
	if( !superFlag ) {
		outputStream << ")";
	}

	if( IsIncludeAllChars() && !chars.IsEmpty() ) {
		outputStream << "(";
	}
	if( !chars.IsEmpty() ) {
		outputStream << "'";
	}
	std::set<TChar> a;
	chars.GetSet( &a );
	for( std::set<TChar>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		outputStream << *i;
	}
	if( !chars.IsEmpty() ) {
		outputStream << "'";
	}
	if( IsIncludeAllChars() && !chars.IsEmpty() ) {
		outputStream << ") ";
	}

	if( IsIncludeAllChars() ) {
		outputStream << "O ";
	} else {
		outputStream << "(O) ";
	}
}

void CQualifier::printLabels( std::ostream& outputStream,
	const CPrintHelper& printHelper ) const
{
	if( IsIncludeAllLabels() && !labels.IsEmpty() ) {
		outputStream << "(";
	}

	std::set<TLabel> a;
	labels.GetSet( &a );
	for( std::set<TLabel>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		outputStream << "/";
		printHelper.Label( outputStream, *i );
		outputStream << "/";
	}

	if( IsIncludeAllLabels() && !labels.IsEmpty() ) {
		outputStream << ")";
	}

	if( IsIncludeAllLabels() ) {
		outputStream << "F ";
	} else {
		outputStream << "(F) ";
	}
}

void CQualifier::printNumbers( std::ostream& outputStream ) const
{
	if( IsIncludeAllNumbers() && !numbers.IsEmpty() ) {
		outputStream << "(";
	}

	std::set<TNumber> a;
	numbers.GetSet( &a );
	for( std::set<TNumber>::const_iterator i = a.begin(); i != a.end(); ++i ) {
		outputStream << "/" << *i << "/";
	}

	if( IsIncludeAllNumbers() && !numbers.IsEmpty() ) {
		outputStream << ")";
	}

	if( IsIncludeAllNumbers() ) {
		outputStream << "N ";
	} else {
		outputStream << "(N) ";
	}
}

//-----------------------------------------------------------------------------

} // end of namespace refal2
