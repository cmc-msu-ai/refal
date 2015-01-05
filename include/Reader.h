#pragma once

#include "QualifierBuilder.h"
#include "FunctionBuilder.h"
#include "Qualifier.h"
#include "Common.h"
#include <iostream>
#include <stack>
#include <map>

namespace Refal2 {

class CReader {
public:
	enum TStatus {
		S_ok,
		S_warnings,
		S_errors
	};

	CReader(CCommonReader& common, std::ostream& error_stream = std::cerr);
	~CReader();

	void Step(char c);
	void End();

	const TStatus& GetStatus() const
	{
		return status;
	}

	static std::string lower(const std::string& s);
	
private:
	CReader(const CReader&);
	CReader& operator=(const CReader&);

	inline void on_warning();
	inline void on_error();

	enum TLexicalErrorCodes {
		LEC_unexpected_control_sequence,
		LEC_symbol_after_plus,
		LEC_unexpected_character,
		LEC_illegal_character_in_label_or_number_begin,
		LEC_illegal_character_in_label,
		LEC_illegal_character_in_number,
		LEC_illegal_character_in_qualifier_name_begin,
		LEC_illegal_character_in_qualifier_name,
		LEC_illegal_character_in_string_after_slash,
		LEC_illegal_character_in_string_in_hexadecimal,
		LEC_trying_append_null_byte_to_string,
		LEC_illegal_character_in_string_in_octal,
		LEC_unclosed_string_constant_at_the_end_of_file,
		LEC_unclosed_string_constant,
		LEC_unclosed_label_or_number_at_the_end_of_file,
		LEC_unclosed_label_or_number,
		LEC_unclosed_label_at_the_end_of_file,
		LEC_unclosed_label,
		LEC_unclosed_number_at_the_end_of_file,
		LEC_unclosed_number,
		LEC_unclosed_qualifier_at_the_end_of_file,
		LEC_unclosed_qualifier,
		LEC_unexpected_end_of_file
	};

	void errorc(TLexicalErrorCodes code, char c = '\0');

	enum TReaderWarningsCodes {
		RW
	};

	bool processLine();

	enum TReaderErrorsCodes {
		RE_cant_create_qualifier,
		RE_unclosed_left_paren_in_qualifier,
		


	};

	enum Lexem {
		LEXEM_BLANK,
		LEXEM_IDENT,
		LEXEM_EQUAL,
		LEXEM_COMMA,
		LEXEM_LABEL,
		LEXEM_NUMBER,
		LEXEM_STRING,
		LEXEM_NEWLINE,
		LEXEM_QUALIFIER,
		LEXEM_LEFT_PAREN,
		LEXEM_RIGHT_PAREN,
		LEXEM_LEFT_BRACKET,
		LEXEM_RIGHT_BRACKET
	};

	void errorl(Lexem lexem);

	struct CScanner {
		CScanner(): state(BEGIN_OF_LINE), offset(0) {}
		
		enum {
			TAB_SIZE = 8
		};

		enum TState {
			BEGIN_OF_LINE,
			BEGIN_BLANK,
			BEGIN_COMMENT,
			BEGIN_PLUS,
			BEGIN_COMMENT_AFTER_PLUS,
			NOT_BEGIN_OF_LINE,
			NOT_BEGIN_BLANK,
			NOT_BEGIN_COMMENT,
			NOT_BEGIN_PLUS,
			NOT_BEGIN_COMMENT_AFTER_PLUS,
			S, SA, SB, SX, S0, SXA, S0A,
			BEGIN_LABEL_OR_NUMBER,
			NOT_BEGIN_LABEL,
			NOT_BEGIN_NUMBER,
			IDENTIFICATOR,
			BEGIN_QUALIFIER,
			NOT_BEGIN_QUALIFIER
		};
		
		TState state;
		int offset;
		int tmp;
	};

	void process_char(char c);
	void process_end();
	
	struct CParser {
		CParser(): state(BEGIN), tmp(0) {}
		
		enum TState
		{
			BEGIN,
			BEGIN_IDENT,  
			BEGIN_BLANK,
			ONLY_NEWLINE,
			WAIT_NEWLINE,
			BEGIN_IDENT_BLANK,
			BEGIN_IDENT_BLANK_S,
		
			BEGIN_ENTRY,
			BEGIN_ENTRY_BLANK,
	
			BEGIN_EMPTY,
			BEGIN_EMPTY_COMMA,
			BEGIN_EMPTY_IDENT,
	
			BEGIN_EXTRN,
			BEGIN_EXTRN_COMMA,
			BEGIN_EXTRN_IDENT,
			BEGIN_EXTRN_LEFT_PAREN,
			BEGIN_EXTRN_INNER_IDENT,
			BEGIN_EXTRN_RIGHT_PAREN,
			BEGIN_EXTRN_ONLY_COMMA,
			
			S_process_qualifier,
			S_process_rule

		};

		TState state;
		int tmp;
	};

	void process_lexem(Lexem lexem);

	bool is_qualifier_name_available();
	void reset_qualifier();
	void save_qualifier();
	CQualifier* create_qualifier();
	bool process_qualifier(Lexem lexem);

	struct CRuleParser {
		CRuleParser() { Reset(); }

		void Reset();        

		enum TState {
			S_begin,
			S_begin_order,
			S_begin_left_part,
			S_begin_right_part,
			S_begin_variable,
			S_begin_qualifier,
			S_begin_variable_qualifier,
			S_begin_left_bracket
		};

		TState state;
		TVariableType variable_type;
		CQualifier* variable_qualifier;
		int paren_balance_counter;
		std::stack<CLink*> stack;
	};

	void reset_current_function();
	bool set_current_function();
	void set_entry_function(const std::string& name);
	void set_empty_function(const std::string& name);
	void set_extrn_function(const std::string& name,
							const std::string& oldname = "");
	bool finalize_left_part();
	bool finalize_rule();
	bool append_string();
	bool append_label();
	bool append_number();
	bool append_left_paren();
	bool append_right_paren();
	bool create_copy_qualifier();
	bool append_variable(TVariableType, TVariableName, CQualifier* = 0);
	bool set_variable_qualifier();
	bool append_left_bracket();
	bool append_right_bracket();
	bool process_function(Lexem lexem);
	   
	CCommonReader& common;
	std::ostream& error_stream;
	TStatus status;

	int line;
	int offset;
	
	TNumber lexem_number;
	std::string lexem_buffer;
	
	CScanner scanner;
	CParser parser;
	
	std::string current_name;
	CQualifierBuilder qualifier_builder;
	bool right_paren_was_last;

	TLabels::iterator current_function;
	CRuleParser rule_parser;
	bool was_error_in_function;
	CFunctionBuilder function_builder;
};

// ----------------------------------------------------------------------------

inline void CReader::on_warning()
{
	if( status != S_errors ) {
		status = S_warnings;
	}
}

inline void CReader::on_error()
{
	if( status != S_errors ) {
		status = S_errors;
	}
}

} // end of namespace Refal2
