#include "QualifierBuilder.h"
#include "SetBuilder.h"
#include "Qualifier.h"
#include <bitset>

namespace Refal2 {

const TAnsiSet CQualifierBuilder::ansi_L = make_ansi_L();
const TAnsiSet CQualifierBuilder::ansi_D = make_ansi_D();

void CQualifierBuilder::make_from_string(TAnsiSet& set, const char* ansi)
{
	for( ; ansi[0] != '\0'; ++ansi ) {
		set.set(static_cast<unsigned int>(ansi[0]));
	}
}

TAnsiSet CQualifierBuilder::make_ansi_L()
{
	TAnsiSet tmp;
	make_from_string(tmp, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	return tmp;
}

TAnsiSet CQualifierBuilder::make_ansi_D()
{
	TAnsiSet tmp;
	make_from_string(tmp, "0123456789");
	return tmp;
}

void CQualifierBuilder::Reset()
{
	negative = false;

	terms = S_none;
	chars = S_none;

	chars_builder.Reset();
	labels_builder.Reset();
	numbers_builder.Reset();

	ansichars.reset();
	ansichars_fixed.reset();
}

void CQualifierBuilder::Get(CQualifier* qualifier)
{
	W();

	qualifier->ansichars = ansichars;
	if( terms == S_yes ) {
		qualifier->flags |= CQualifier::F_terms;
	}
	bool tmp;
	chars_builder.Export(qualifier->chars, tmp);
	if( tmp ) {
		qualifier->flags |= CQualifier::F_all_chars;
	}
	labels_builder.Export(qualifier->labels, tmp);
	if( tmp ) {
		qualifier->flags |= CQualifier::F_all_labels;
	}
	numbers_builder.Export(qualifier->numbers, tmp);
	if( tmp ) {
		qualifier->flags |= CQualifier::F_all_numbers;
	}
}

void CQualifierBuilder::Label(TLabel label)
{
	if( negative ) {
		labels_builder.Exclude(label);
	} else {
		labels_builder.Include(label);
	}
}

void CQualifierBuilder::Number(TNumber number)
{
	if( negative ) {
		numbers_builder.Exclude(number);
	} else {
		numbers_builder.Include(number);
	}
}

void CQualifierBuilder::Char(TChar c)
{
	if( chars == S_none ) {
		if( c >= 0 && c < D_ansi_set_size ) {
			if( !ansichars_fixed.test(static_cast<unsigned int>(c)) ) {
				ansichars.set(static_cast<unsigned int>(c), !negative);
				ansichars_fixed.set(static_cast<unsigned int>(c));
			}
		} else {
			if( negative ) {
				chars_builder.Exclude(c);
			} else {
				chars_builder.Include(c);
			}
		}
	}
}

void CQualifierBuilder::Qualifier(const CQualifier& qualifier)
{
	if( chars == S_none ) {
		if( negative ) {
			/* ansichars */
			ansichars &= ansichars_fixed | (~qualifier.ansichars);
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				chars_builder.ExcludeAllExcept(qualifier.chars);
			} else {
				chars_builder.Exclude(qualifier.chars);
			}
		} else {
			/* ansichars */
			ansichars |= (~ansichars_fixed) & qualifier.ansichars;
			/* chars */
			if( qualifier.IsIncludeAllChars() ) {
				chars_builder.IncludeAllExcept(qualifier.chars);
			} else {
				chars_builder.Include(qualifier.chars);
			}
		}
	}
	
	if( negative ) {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labels_builder.ExcludeAllExcept(qualifier.labels);
		} else {
			labels_builder.Exclude(qualifier.labels);
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbers_builder.ExcludeAllExcept(qualifier.numbers);
		} else {
			numbers_builder.Exclude(qualifier.numbers);
		}
	} else {
		/* labels */
		if( qualifier.IsIncludeAllLabels() ) {
			labels_builder.IncludeAllExcept(qualifier.labels);
		} else {
			labels_builder.Include(qualifier.labels);
		}
		/* numbers */
		if( qualifier.IsIncludeAllNumbers() ) {
			numbers_builder.IncludeAllExcept(qualifier.numbers);
		} else {
			numbers_builder.Include(qualifier.numbers);
		}
	}

	/* terms */
	if( terms == S_none && qualifier.IsIncludeTerms() ) {
		terms = negative ? S_no : S_yes;
	}
}

void CQualifierBuilder::Negative()
{
	negative = !negative;
}

void CQualifierBuilder::F()
{
	if( negative ) {
		labels_builder.ExcludeAll();
	} else {
		labels_builder.IncludeAll();
	}
}

void CQualifierBuilder::N()
{
	if( negative ) {
		numbers_builder.ExcludeAll();
	} else {
		numbers_builder.IncludeAll();
	}
}

void CQualifierBuilder::O()
{
	if( chars == S_none ) {
		if( negative ) {
			chars = S_no;
			ansichars &= ansichars_fixed;
			chars_builder.ExcludeAll();
		} else {
			chars = S_yes;
			ansichars |= ~ansichars_fixed;
			chars_builder.IncludeAll();
		}
	}
}

void CQualifierBuilder::L()
{
	if( chars == S_none ) {
		if( negative ) {
			ansichars &= ~ansi_L | ansichars_fixed;
		} else {
			ansichars |= ansi_L & (~ansichars_fixed);
		}
		ansichars_fixed |= ansi_L;
	}
}

void CQualifierBuilder::D()
{
	if( chars == S_none ) {
		if( negative ) {
			ansichars &= ~ansi_D | ansichars_fixed;
		} else {
			ansichars |= ansi_D & (~ansichars_fixed);
		}
		ansichars_fixed |= ansi_D;
	}
}

void CQualifierBuilder::B()
{
	if( terms == S_none ) {
		terms = negative ? S_no : S_yes;
	}
}

} // end of namespace refal2

