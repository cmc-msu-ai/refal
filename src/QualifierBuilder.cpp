#include <bitset>
#include <Refal2.h>

namespace Refal2 {

const char* CQualifierBuilder::Alphabet = "abcdefghijklmnopqrstuvwxyz"
											"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* CQualifierBuilder::Numbers = "0123456789";

const TAnsiSet CQualifierBuilder::AnsiL = MakeFromString( Alphabet );
const TAnsiSet CQualifierBuilder::AnsiD = MakeFromString( Numbers );

TAnsiSet CQualifierBuilder::MakeFromString(const char* ansiString)
{
	TAnsiSet set;
	for( ; ansiString[0] != '\0'; ++ansiString ) {
		set.set(static_cast<unsigned int>(ansiString[0]));
	}
	return set;
}

void CQualifierBuilder::Reset()
{
	negative = false;

	terms = S_none;
	chars = S_none;

	charsBuilder.Reset();
	labelsBuilder.Reset();
	numbersBuilder.Reset();

	ansichars.reset();
	ansicharsFixed.reset();
}

void CQualifierBuilder::Export(CQualifier* qualifier)
{
	AddW();

	qualifier->flags = 0;
	qualifier->ansichars = ansichars;
	if( terms == S_yes ) {
		qualifier->flags |= QIF_Terms;
	}

	bool isIncludeAllChars = false;
	charsBuilder.Export( &qualifier->chars, &isIncludeAllChars );
	if( isIncludeAllChars ) {
		qualifier->flags |= QIF_AllChars;
	}

	bool isIncludeAllLabels = false;
	labelsBuilder.Export( &qualifier->labels, &isIncludeAllLabels );
	if( isIncludeAllLabels ) {
		qualifier->flags |= QIF_AllLabels;
	}

	bool isIncludeAllNumbers = false;
	numbersBuilder.Export( &qualifier->numbers, &isIncludeAllNumbers);
	if( isIncludeAllNumbers ) {
		qualifier->flags |= QIF_AllNumbers;
	}
}

void CQualifierBuilder::AddChar(TChar c)
{
	if( chars == S_none ) {
		if( c >= 0 && static_cast<int>(c) < AnsiSetSize ) {
			if( !ansicharsFixed.test(static_cast<unsigned int>(c)) ) {
				ansichars.set(static_cast<unsigned int>(c), !negative);
				ansicharsFixed.set(static_cast<unsigned int>(c));
			}
		} else {
			if( negative ) {
				charsBuilder.Exclude(c);
			} else {
				charsBuilder.Include(c);
			}
		}
	}
}

void CQualifierBuilder::AddLabel(TLabel label)
{
	if( negative ) {
		labelsBuilder.Exclude(label);
	} else {
		labelsBuilder.Include(label);
	}
}

void CQualifierBuilder::AddNumber(TNumber number)
{
	if( negative ) {
		numbersBuilder.Exclude(number);
	} else {
		numbersBuilder.Include(number);
	}
}

void CQualifierBuilder::AddQualifier(const CQualifier& qualifier)
{
	if( chars == S_none ) {
		if( negative ) {
			/* ansichars */
			ansichars &= ansicharsFixed | (~qualifier.ansichars);
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				charsBuilder.ExcludeAllExcept(qualifier.chars);
			} else {
				charsBuilder.Exclude(qualifier.chars);
			}
		} else {
			/* ansichars */
			ansichars |= (~ansicharsFixed) & qualifier.ansichars;
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				charsBuilder.IncludeAllExcept(qualifier.chars);
			} else {
				charsBuilder.Include(qualifier.chars);
			}
		}
	}
	
	if( negative ) {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labelsBuilder.ExcludeAllExcept(qualifier.labels);
		} else {
			labelsBuilder.Exclude(qualifier.labels);
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbersBuilder.ExcludeAllExcept(qualifier.numbers);
		} else {
			numbersBuilder.Exclude(qualifier.numbers);
		}
	} else {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labelsBuilder.IncludeAllExcept(qualifier.labels);
		} else {
			labelsBuilder.Include(qualifier.labels);
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbersBuilder.IncludeAllExcept(qualifier.numbers);
		} else {
			numbersBuilder.Include(qualifier.numbers);
		}
	}

	/* terms */
	if( terms == S_none && qualifier.IsIncludeTerms() ) {
		terms = negative ? S_no : S_yes;
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
	if( chars == S_none ) {
		if( negative ) {
			chars = S_no;
			ansichars &= ansicharsFixed;
			charsBuilder.ExcludeAll();
		} else {
			chars = S_yes;
			ansichars |= ~ansicharsFixed;
			charsBuilder.IncludeAll();
		}
	}
}

void CQualifierBuilder::AddL()
{
	if( chars == S_none ) {
		if( negative ) {
			ansichars &= ~AnsiL | ansicharsFixed;
		} else {
			ansichars |= AnsiL & (~ansicharsFixed);
		}
		ansicharsFixed |= AnsiL;
	}
}

void CQualifierBuilder::AddD()
{
	if( chars == S_none ) {
		if( negative ) {
			ansichars &= ~AnsiD | ansicharsFixed;
		} else {
			ansichars |= AnsiD & (~ansicharsFixed);
		}
		ansicharsFixed |= AnsiD;
	}
}

void CQualifierBuilder::AddB()
{
	if( terms == S_none ) {
		terms = negative ? S_no : S_yes;
	}
}

} // end of namespace refal2
