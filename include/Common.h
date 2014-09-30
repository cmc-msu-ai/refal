#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdint.h>

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

typedef TLabels::value_type* TLabel;
typedef TQualifiers::value_type* TQualifier;

// ----------------------------------------------------------------------------

struct CUnitLink;
struct CVariableLink;

struct CLink {
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

	CLink(TUnitType _type): type(_type), next(0), prev(0) {}

	TUnitType Type() const { return type; }

	bool IsVariable() const { return ( (type & T_variable) != 0 ); }
	inline CVariableLink* Variable();
	inline const CVariableLink* Variable() const;

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
	
/*private:*/
	TUnitType type;
	CLink* next;
	CLink* prev;
};

struct CUnitLink : public CLink {
	CUnitLink(TUnitType type): CLink(type) {}

	inline CUnitLink* Next();
	inline const CUnitLink* Next() const;

	inline CUnitLink* Prev();
	inline const CUnitLink* Prev() const;	

	union CUnitValue {
		TChar c;
		TLabel label;
		TNumber number;
		CUnitLink* paired_paren;
	};

	CUnitValue value;
};

struct CUnitValue {
	CUnitValue(CLink::TUnitType _type):
		type(_type) {}
	CUnitValue(const CUnitLink& unit_link):
		type(unit_link.type), value(unit_link.value) {}

	CLink::TUnitType type;
	CUnitLink::CUnitValue value;
};

inline bool operator==(const CUnitLink& link, const CUnitValue& value)
{
	switch( link.type ) {
		case CLink::T_char:
			return ( value.type == CLink::T_char &&
				link.value.c == value.value.c );
			break;
		case CLink::T_label:
			return ( value.type == CLink::T_label &&
				link.value.label == value.value.label );
			break;
		case CLink::T_number:
			return ( value.type == CLink::T_number &&
				link.value.number == value.value.number );
		default:
			return ( link.type == value.type &&
				link.value.paired_paren == value.value.paired_paren );
			break;
	}
	/* assert */
}

inline bool operator!=(const CUnitLink& link, const CUnitValue& value)
{
	return !( link == value );
}

struct CVariableLink : public CLink {
	CVariableLink(TVariableType _type = '\0', TVariableName _name = '\0',
			CQualifier* _qualifier = 0):
		CLink(T_variable),
		variable_type(_type),
		variable_name(_name),
		qualifier(_qualifier)
	{
	}
	
	TVariableType variable_type;
	TVariableName variable_name;
	CQualifier* qualifier;
};

inline CVariableLink* CLink::Variable()
{
	return static_cast<CVariableLink*>(this);
}

inline const CVariableLink* CLink::Variable() const
{
	return static_cast<const CVariableLink*>(this);
}

inline TChar& CLink::Char()
{
	return static_cast<CUnitLink*>(this)->value.c;
}

inline const TChar& CLink::Char() const
{
	return static_cast<const CUnitLink*>(this)->value.c;
}

inline TLabel& CLink::Label()
{
	return static_cast<CUnitLink*>(this)->value.label;
}

inline const TLabel& CLink::Label() const
{
	return static_cast<const CUnitLink*>(this)->value.label;
}

inline TNumber& CLink::Number()
{
	return static_cast<CUnitLink*>(this)->value.number;
}

inline const TNumber& CLink::Number() const
{
	return static_cast<const CUnitLink*>(this)->value.number;
}

inline CUnitLink* CLink::Unit()
{
	return static_cast<CUnitLink*>(this);
}

inline const CUnitLink* CLink::Unit() const
{
	return static_cast<const CUnitLink*>(this);
}

inline CUnitLink*& CLink::PairedParen()
{
	return static_cast<CUnitLink*>(this)->value.paired_paren;
}

inline const CUnitLink* CLink::PairedParen() const
{
	return static_cast<const CUnitLink*>(this)->value.paired_paren;
}

inline CUnitLink* CUnitLink::Next()
{
	return reinterpret_cast<CUnitLink*>(next);
}

inline const CUnitLink* CUnitLink::Next() const
{
	return reinterpret_cast<const CUnitLink*>(next);
}

inline CUnitLink* CUnitLink::Prev()
{
	return reinterpret_cast<CUnitLink*>(prev);
}

inline const CUnitLink* CUnitLink::Prev() const
{
	return reinterpret_cast<const CUnitLink*>(prev);
}

// ----------------------------------------------------------------------------

struct CRule
{
	CLink* left_part_begin;
	CLink* left_part_end;
	CLink* right_part_begin;
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

void print_link(const CLink* link);
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

