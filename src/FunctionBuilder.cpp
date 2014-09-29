#include "FunctionBuilder.h"

#include "LinkBuilder.h"
#include "Common.h"

namespace Refal2 {

void CFunctionBuilder::Free(CRule** rule)
{
	CRule* i = *rule;
	*rule = 0;

	while( i != 0 ) {
		CRule* tmp = i;
		i = i->next_rule;

		CLinkBuilder::Free( &(tmp->left_part_begin) );
		CLinkBuilder::Free( &(tmp->right_part_begin) );
	}
}

void CFunctionBuilder::Reset()
{
	if( begin != 0 ) {
		Free(&begin);
	}

	begin = 0;
	end = 0;
	current_rule_was_created = false;
}

void CFunctionBuilder::Get(CRule** rule)
{
	if( rule != 0 ) {
		if( current_rule_was_created ) {
			EndOfRule();
		}

		*rule = begin;
		begin = 0;
		Reset();
	}
}

void CFunctionBuilder::RightDirection()
{
	if( !current_rule_was_created ) {
		rule();
	}
	end->right_matching = true;
}

void CFunctionBuilder::EndOfLeftPart()
{
	if( !current_rule_was_created ) {
		rule();
	}
	CLinkBuilder::Get( &(end->left_part_begin), &(end->left_part_end) );
}

void CFunctionBuilder::EndOfRule()
{
	if( !current_rule_was_created ) {
		/* ERROR */
	} else {
		CLinkBuilder::Get( &(end->right_part_begin), 0 );
		current_rule_was_created = false;
	}
}

void CFunctionBuilder::rule()
{
	current_rule_was_created = true;
	CRule* rule = new CRule();
	if( begin == 0 ) {
		begin = rule;
		end = begin;
	} else {
		end->next_rule = rule;
		end = rule;
	}
}

} // end of namespace Refal2

