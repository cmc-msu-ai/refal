#include <bitset>
#include <Refal2.h>

namespace Refal2 {

const char* CQualifierBuilder::Alphabet = "abcdefghijklmnopqrstuvwxyz"
											"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* CQualifierBuilder::Numbers = "0123456789";

const CAnsiSet CQualifierBuilder::AnsiL = MakeFromString( Alphabet );
const CAnsiSet CQualifierBuilder::AnsiD = MakeFromString( Numbers );

CAnsiSet CQualifierBuilder::MakeFromString( const char* ansiString )
{
	CAnsiSet set;
	for( ; ansiString[0] != '\0'; ansiString++ ) {
		set.set( static_cast<unsigned int>( ansiString[0] ) );
	}
	return set;
}

void CQualifierBuilder::Reset()
{
	negative = false;

	terms = S_None;
	chars = S_None;

	charsBuilder.Reset();
	labelsBuilder.Reset();
	numbersBuilder.Reset();

	ansichars.reset();
	ansicharsFixed.reset();
}

void CQualifierBuilder::Export( CQualifier& qualifier )
{
	AddW();
	qualifier.flags = 0;
	qualifier.ansichars = ansichars;
	if( terms == S_Yes ) {
		qualifier.flags |= QIF_Terms;
	}
	if( charsBuilder.Export( qualifier.chars ) ) {
		qualifier.flags |= QIF_AllChars;
	}
	if( labelsBuilder.Export( qualifier.labels ) ) {
		qualifier.flags |= QIF_AllLabels;
	}
	if( numbersBuilder.Export( qualifier.numbers ) ) {
		qualifier.flags |= QIF_AllNumbers;
	}
}

void CQualifierBuilder::AddChar( const TChar c )
{
	if( chars == S_None ) {
		if( c >= 0 && static_cast<int>(c) < AnsiSetSize ) {
			if( !ansicharsFixed.test( static_cast<unsigned int>( c ) ) ) {
				ansichars.set( static_cast<unsigned int>( c ), !negative );
				ansicharsFixed.set( static_cast<unsigned int>( c ) );
			}
		} else {
			if( negative ) {
				charsBuilder.Exclude( c );
			} else {
				charsBuilder.Include( c );
			}
		}
	}
}

void CQualifierBuilder::AddLabel( const TLabel label )
{
	if( negative ) {
		labelsBuilder.Exclude( label );
	} else {
		labelsBuilder.Include( label );
	}
}

void CQualifierBuilder::AddNumber( const TNumber number )
{
	if( negative ) {
		numbersBuilder.Exclude( number );
	} else {
		numbersBuilder.Include( number );
	}
}

void CQualifierBuilder::AddQualifier( const CQualifier& qualifier )
{
	if( chars == S_None ) {
		if( negative ) {
			/* ansichars */
			ansichars &= ansicharsFixed | ~qualifier.ansichars;
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				charsBuilder.ExcludeAllExcept( qualifier.chars );
			} else {
				charsBuilder.Exclude( qualifier.chars );
			}
		} else {
			/* ansichars */
			ansichars |= ~ansicharsFixed & qualifier.ansichars;
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				charsBuilder.IncludeAllExcept( qualifier.chars );
			} else {
				charsBuilder.Include( qualifier.chars );
			}
		}
	}
	if( negative ) {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labelsBuilder.ExcludeAllExcept( qualifier.labels );
		} else {
			labelsBuilder.Exclude( qualifier.labels );
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbersBuilder.ExcludeAllExcept( qualifier.numbers );
		} else {
			numbersBuilder.Exclude( qualifier.numbers );
		}
	} else {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labelsBuilder.IncludeAllExcept( qualifier.labels );
		} else {
			labelsBuilder.Include( qualifier.labels );
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbersBuilder.IncludeAllExcept( qualifier.numbers );
		} else {
			numbersBuilder.Include( qualifier.numbers );
		}
	}
	/* terms */
	if( terms == S_None && qualifier.IsIncludeTerms() ) {
		terms = negative ? S_No : S_Yes;
	}
}

void CQualifierBuilder::AddF()
{
	if( negative ) {
		labelsBuilder.ExcludeAll();
	} else {
		labelsBuilder.IncludeAll();
	}
}

void CQualifierBuilder::AddN()
{
	if( negative ) {
		numbersBuilder.ExcludeAll();
	} else {
		numbersBuilder.IncludeAll();
	}
}

void CQualifierBuilder::AddO()
{
	if( chars == S_None ) {
		if( negative ) {
			chars = S_No;
			ansichars &= ansicharsFixed;
			charsBuilder.ExcludeAll();
		} else {
			chars = S_Yes;
			ansichars |= ~ansicharsFixed;
			charsBuilder.IncludeAll();
		}
	}
}

void CQualifierBuilder::AddL()
{
	if( chars == S_None ) {
		if( negative ) {
			ansichars &= ~AnsiL | ansicharsFixed;
		} else {
			ansichars |= AnsiL & ~ansicharsFixed;
		}
		ansicharsFixed |= AnsiL;
	}
}

void CQualifierBuilder::AddD()
{
	if( chars == S_None ) {
		if( negative ) {
			ansichars &= ~AnsiD | ansicharsFixed;
		} else {
			ansichars |= AnsiD & ~ansicharsFixed;
		}
		ansicharsFixed |= AnsiD;
	}
}

void CQualifierBuilder::AddB()
{
	if( terms == S_None ) {
		terms = negative ? S_No : S_Yes;
	}
}

} // end of namespace refal2
