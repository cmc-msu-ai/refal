#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdint.h>
#include "Variables.h"

namespace Refal2 {

enum TDefaults
{
	D_ansi_set_size = 128
};

typedef char TVariableType;
typedef char TVariableName;

struct CRule;
struct COperation;

struct CFunction
{
	CRule* rule;
	COperation* operation;

	CFunction(CRule* _rule = 0): rule(_rule), operation(0) {}
};

class CQualifier;

typedef std::map<std::string, CFunction> TLabels;
typedef std::map<std::string, CQualifier*> TQualifiers;

typedef int_fast32_t TNumber;
typedef char TChar;

//typedef TLabels::value_type* TLabel;
typedef TQualifiers::value_type* TQualifier;

// ----------------------------------------------------------------------------

struct CUnitLink {
	enum TUnitType {
		T_char = 0x01,
		T_label = 0x02,
		T_number = 0x04,
		T_variable = 0x08,
		T_left_paren = 0x10,
		T_right_paren = 0x20,
		T_left_bracket = 0x40,
		T_right_bracket = 0x80
	};

	enum {
		T_paren = T_left_paren | T_right_paren,
		T_symbol = T_char | T_label | T_number,
		T_unit = T_paren | T_symbol,
		T_bracket = T_left_bracket | T_right_bracket
	};

	CUnitLink(TUnitType _type): type(_type), next(0), prev(0) {}

	TUnitType Type() const { return type; }

	bool IsVariable() const { return ( (type & T_variable) != 0 ); }
	inline TVariableIndex& Variable();
	inline const TVariableIndex& Variable() const;

	bool IsChar() const { return ( (type & T_char) != 0 ); }
	inline TChar& Char();
	inline const TChar& Char() const;

	bool IsLabel() const { return ( (type & T_label) != 0 ); }
	inline TLabel& Label();
	inline const TLabel& Label() const;

	bool IsNumber() const { return ( (type & T_number) != 0 ); }
	inline TNumber& Number();
	inline const TNumber& Number() const;

	bool IsSymbol() const { return ( (type & T_symbol) != 0 ); }

	bool IsUnit() const { return ( (type & T_unit) != 0 ); }
	inline CUnitLink* Unit();
	inline const CUnitLink* Unit() const;

	bool IsParen() const { return ( (type & T_paren) != 0 ); }
	bool IsLeftParen() const { return ( (type & T_left_paren) != 0 ); }
	bool IsRightParen() const { return ( (type & T_right_paren) != 0 ); }
	inline CUnitLink*& PairedParen();
	inline const CUnitLink* PairedParen() const;

	bool IsBracket() const { return ( (type & T_bracket) != 0 ); }
	bool IsLeftBracket() const { return ( (type & T_left_bracket) != 0 ); }
	bool IsRightBracket() const { return ( (type & T_right_bracket) != 0 ); }
	
	inline CUnitLink* Next();
	inline const CUnitLink* Next() const;

	inline CUnitLink* Prev();
	inline const CUnitLink* Prev() const;	

/*private:*/
	TUnitType type;
	union {
		TChar c;
		TLabel label;
		TNumber number;
		TVariableIndex variable;
		CUnitLink* pairedParen;
	};
	CUnitLink* next;
	CUnitLink* prev;
};

inline TVariableIndex& CUnitLink::Variable()
{
	return variable;
}

inline const TVariableIndex& CUnitLink::Variable() const
{
	return variable;
}

inline TChar& CUnitLink::Char()
{
	return c;
}

inline const TChar& CUnitLink::Char() const
{
	return c;
}

inline TLabel& CUnitLink::Label()
{
	return label;
}

inline const TLabel& CUnitLink::Label() const
{
	return label;
}

inline TNumber& CUnitLink::Number()
{
	return number;
}

inline const TNumber& CUnitLink::Number() const
{
	return number;
}


inline CUnitLink*& CUnitLink::PairedParen()
{
	return pairedParen;
}

inline const CUnitLink* CUnitLink::PairedParen() const
{
	return pairedParen;
}

inline CUnitLink* CUnitLink::Next()
{
	return next;
}

inline const CUnitLink* CUnitLink::Next() const
{
	return next;
}

inline CUnitLink* CUnitLink::Prev()
{
	return prev;
}

inline const CUnitLink* CUnitLink::Prev() const
{
	return prev;
}

// ----------------------------------------------------------------------------

struct CRule
{
	CUnitLink* left_part_begin;
	CUnitLink* left_part_end;
	CUnitLink* right_part_begin;
	CVariables* variables;
	CRule* next_rule;
	bool right_matching;

	CRule():
		left_part_begin(0),
		left_part_end(0),
		right_part_begin(0),
		next_rule(0),
		right_matching(false)
	{
	}
};

void print_link(const CUnitLink* link);
void print_rule(const CRule* rule);
void print_function(const CRule* rule);

struct CCommonReader {
public:
	CCommonReader():
		labels(),
		qualifiers(),
		entry(0),
		EMPTY_FUNCTION(new CRule[0]),
		EXTRN_FUNCTION(new CRule[0])
	{
	}
	~CCommonReader()
	{
		delete[] EMPTY_FUNCTION;
		delete[] EXTRN_FUNCTION;
	}

	TLabels labels;
	TQualifiers qualifiers;
	TLabel entry;

	CRule* const EMPTY_FUNCTION;
	CRule* const EXTRN_FUNCTION;

private:
	CCommonReader(const CCommonReader&);
	CCommonReader& operator=(const CCommonReader&);
};

} // end of namespace Refal2

