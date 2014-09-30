#if 1
#include "OperationsBuilder.h"
#include "QualifierBuilder.h"
#include "FunctionBuilder.h"
#include "FieldOfView.h"
#include "Qualifier.h"
#include "Executer.h"
#include "FastSet.h"
#include "Common.h"
#include <iostream>
#include <fstream>

using namespace Refal2;

int main(int argc, const char* argv[])
{
	CQualifier a;
	
	{
		CQualifierBuilder tmp;
		tmp.Char('H');
		tmp.Char('e');
		tmp.Char('l');
		tmp.B();
		tmp.N();
		tmp.Negative();
		tmp.Get(&a);
	}
	/*
	CFunctionBuilder bf;
	bf.Char('a');
	bf.LeftParen();
	bf.Char('b');
	bf.RightParen();
	bf.EndOfLeftPart();
	bf.Number(444);
	bf.Char('c');
	bf.Number(7);
	bf.EndOfRule();

	CRule* rule;
	bf.Get(&rule);

	print_function(rule);*/

	COperationsBuilder builder;

	CFieldOfView view;

	CUnitLink* left = view.First();
	CUnitLink* right = view.Last();
	right = CFieldOfView::Insert(view.First(), CUnitValue(CLink::T_right_paren));
	left = CFieldOfView::Insert(view.First(), CUnitValue(CLink::T_left_paren));
	left->PairedParen() = right;
	right->PairedParen() = left;

	CUnitLink char_value(CUnitLink::T_char);
	CUnitLink number_value(CUnitLink::T_number);
	CUnitLink left_paren(CUnitLink::T_left_paren);
	CUnitLink right_paren(CUnitLink::T_right_paren);

	COperation* func = builder.Add(COperation::OT_left_max_qualifier, &a);
	builder.Add(COperation::OT_closed_e_variable_match);
	builder.Add(COperation::OT_matching_complete);
	char_value.Char() = '%';
	builder.Add(COperation::OT_insert_symbol, CUnitValue(char_value));
	builder.Add(COperation::OT_move_e, 1);
	char_value.Char() = '$';
	builder.Add(COperation::OT_insert_symbol, CUnitValue(char_value));
	builder.Add(COperation::OT_copy_wve, 3);
	char_value.Char() = '*';
	builder.Add(COperation::OT_insert_symbol, CUnitValue(char_value));
	builder.Add(COperation::OT_move_e, 3);
	char_value.Char() = '#';
	builder.Add(COperation::OT_insert_symbol, CUnitValue(char_value));
	builder.Add(COperation::OT_return);

	COperation* go = builder.Add(COperation::OT_matching_complete);
	builder.Add(COperation::OT_insert_left_paren);
	CUnitLink label_func(CLink::T_label);
	label_func.Label() = new TLabels::value_type;
	const_cast<std::string&>(label_func.Label()->first) = "Mama";
	label_func.Label()->second.operation = func;
	builder.Add(COperation::OT_insert_symbol, label_func);
	const char* tmp = "Hello, World!";
	for( const char* i = tmp; i[0] != '\0'; ++i ) {
		char_value.Char() = i[0];
		number_value.Number() = i - tmp;
		builder.Add(COperation::OT_insert_symbol, char_value);
		builder.Add(COperation::OT_insert_left_paren);
		builder.Add(COperation::OT_insert_symbol, number_value);
		builder.Add(COperation::OT_insert_right_paren);
	}
	builder.Add(COperation::OT_insert_right_bracket);
	builder.Add(COperation::OT_insert_left_paren);
	builder.Add(COperation::OT_insert_symbol, label_func);
	builder.Add(COperation::OT_insert_right_bracket);
	builder.Add(COperation::OT_return);

	CExecuter exe;

	exe.SetStackSize(16);
	exe.SetTableSize(128);

	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";
	exe.Run(go, left, right);
	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";
	/*exe.Run(func, view);
	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";*/

	std::cin.get();

	return 0;
}

#else

#include "Common.h"
#include "Reader.h"
#include <iostream>
#include <fstream>

int main(int argc, const char* argv[])
{
	const char* const default_filename = "test_program.ref";
	const char* filename = default_filename;

	if( argc >= 2 ) {
		filename = argv[1];
	}

	std::ifstream f(filename);

	if( !f.good() )
	{
		std::cerr << "Can't open file\n";
		return 1;
	}

	Refal2::CCommonReader common;
	Refal2::CReader reader(common);

	while( true ) {
		int c = f.get();
		if( c == -1 ) {
			reader.End();
			break;
		} else {
			reader.Step(c);
		}
	}

	if( reader.GetStatus() != Refal2::CReader::S_ok ) {
		return 1;
	}

	for( Refal2::TLabels::const_iterator i = common.labels.begin();
		i != common.labels.end(); ++i )
	{
		if( i->second.rule != 0 &&
			i->second.rule != common.EMPTY_FUNCTION &&
			i->second.rule != common.EXTRN_FUNCTION )
		{
			std::cout << "FUNCTION: " << i->first << "\n";
			print_function(i->second.rule);
			std::cout << "\n\n";
		}
	}

	std::cin.get();

	return 0;
}
#endif
