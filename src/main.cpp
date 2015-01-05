#include <fstream>
#include <iostream>
#include <Refal2.h>

using namespace Refal2;

class CMyClass : public IScannerListener,
	public IParserListener,
	public IFunctionBuilderListener
{
public:
	virtual void OnScannerError(const TScannerErrorCodes errorCode, char c) {}
	virtual void OnParserError(const TParserErrorCodes errorCode) {}
	virtual void OnFunctionBuilderError(const TFunctionBuilderErrorCodes errorCode);
	virtual void OnVariablesBuilderError(const TVariablesBuilderErrorCodes errorCode);
};

void CMyClass::OnFunctionBuilderError(const Refal2::TFunctionBuilderErrorCodes errorCode)
{
	static const char* errorText[] = {
		"FBEC_ThereAreNoRulesInFunction",
		"FBEC_IllegalLeftBracketInLeftPart",
		"FBEC_IllegalRightBracketInLeftPart",
		"FBEC_RightParenDoesNotMatchLeftParen",
		"FBEC_RightBracketDoesNotMatchLeftBracket",
		"FBEC_UnclosedLeftParenInLeftPart",
		"FBEC_UnclosedLeftParenInRightPart",
		"FBEC_UnclosedLeftBracketInRightPart",
		"FBEC_IllegalQualifierInRightPart"
	};
	
	printf("CFunctionBuilder error: %s\n", errorText[errorCode]);
}

void CMyClass::OnVariablesBuilderError(const Refal2::TVariablesBuilderErrorCodes errorCode)
{
	static const char* errorText[] = {
		"VBEC_NoSuchTypeOfVariable",
		"VBEC_TypeOfVariableDoesNotMatch",
		"VBEC_NoSuchVariableInLeftPart"
	};
	
	printf("CVariablesBuilder error: %s\n", errorText[errorCode]);
}

int main(int argc, const char* argv[])
{
	try {
		const char* filename = "C:\\Users\\Антон\\YandexDisk\\MyFiles\\Education\\3 курс\\5 семестр\\практикум на эвм\\Интерпретация паскаль-программы\\PROGRAM__.REF";

		if( argc == 2 ) {
			filename = argv[1];
		}

		CMyClass ca;
		CParser parser( &ca );

		std::ifstream f( filename );
		if( !f.good() ) {
			std::cerr << "Can't open file\n";
			return 1;
		}

		while( true ) {
			int c = f.get();
			if( c == -1 ) {
				parser.AddEndOfFile();
				break;
			} else {
				parser.AddChar(c);
			}
		}
	} catch(bool) {
		return 1;
	}

	return 0;
}

#if 0
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

	CUnitLink* left;
	CUnitLink* right;
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

	COperation* go = builder.Add(COperation::OT_empty_expression_match);
	builder.Add(COperation::OT_matching_complete); /* = */
	builder.Add(COperation::OT_insert_left_paren); /* < */
	CUnitLink label_func(CLink::T_label);
	label_func.Label() = new TLabels::value_type;
	const_cast<std::string&>(label_func.Label()->first) = "Mama";
	label_func.Label()->second.operation = func;
	builder.Add(COperation::OT_insert_symbol, label_func); /* /Mama/ */
	const char* tmp = "Hello, World!";
	for( const char* i = tmp; i[0] != '\0'; ++i ) {
		char_value.Char() = i[0];
		number_value.Number() = i - tmp;
		builder.Add(COperation::OT_insert_symbol, char_value);
		builder.Add(COperation::OT_insert_left_paren);
		builder.Add(COperation::OT_insert_symbol, number_value);
		builder.Add(COperation::OT_insert_right_paren);
	}
	builder.Add(COperation::OT_insert_right_bracket); /* > */
	builder.Add(COperation::OT_insert_left_paren); /* < */
	builder.Add(COperation::OT_insert_symbol, label_func); /* /Mama/ */
	builder.Add(COperation::OT_insert_right_bracket); /* > */
	builder.Add(COperation::OT_return);

	CUnitLink* location = left;
	for( const char* i = tmp; i[0] != '\0'; ++i ) {
		char_value.Char() = i[0];
		location = CFieldOfView::Insert(location, char_value);
	}
	
	COperation* jump = builder.Add(COperation::OT_insert_jump, static_cast<COperation*>(0));
	COperation* reverse = jump;
	builder.Add(COperation::OT_left_s_variable_match);
	CUnitLink label_reverse(CLink::T_label);
	label_reverse.Label() = new TLabels::value_type;
	const_cast<std::string&>(label_reverse.Label()->first) = "reverse";
	label_reverse.Label()->second.operation = reverse;
	builder.Add(COperation::OT_closed_e_variable_match);
	builder.Add(COperation::OT_matching_complete);
	builder.Add(COperation::OT_insert_left_paren); /* < */
	builder.Add(COperation::OT_insert_symbol, label_reverse); /* /reverse/ */
	builder.Add(COperation::OT_move_e, 2);
	builder.Add(COperation::OT_insert_right_bracket); /* > */
	builder.Add(COperation::OT_move_s, 0);
	builder.Add(COperation::OT_return);
	jump->Operation()->operation =
		builder.Add(COperation::OT_empty_expression_match);
	builder.Add(COperation::OT_matching_complete);
	builder.Add(COperation::OT_return);

	CExecuter exe;

	exe.SetStackSize(1024);
	exe.SetTableSize(128);

	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";
	exe.Run(reverse, left, right);
	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";
	/*exe.Run(func, view);
	std::cout << "\n-----\n";
	view.Print();
	std::cout << "\n-----\n";*/
	
	return 0;
}
#endif
#if 0

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
