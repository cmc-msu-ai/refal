#ifndef REFAL2_H
#define REFAL2_H

//#include "temporary_struct.h"

#include <map>
#include <string>
#include <iostream>
#include <fstream>

namespace refal2
{

struct Rule;
struct Operation;
struct Qualifier;

struct Function
{
public:
	union
	{
		Rule*      rule;
		Operation* operation;
	};
};

class Refal2;

typedef std::map<std::string, Function>   LabelsTable;
typedef std::map<std::string, Qualifier*> QualifiersTable;
typedef LabelsTable::value_type           LabelPair;
typedef QualifiersTable::value_type       QualifierPair;

enum Type
{
	LABEL         = 0x01,
	CHAR          = 0x02,
	NUMBER        = 0x04,
	LEFT_PAREN    = 0x08,
	RIGHT_PAREN   = 0x10,
	VARIABLE      = 0x20,
	LEFT_BRACKET  = 0x40,
	RIGHT_BRACKET = 0x80
};

enum
{
	SYMBOL        = LABEL | CHAR | NUMBER,
	PAREN         = LEFT_PAREN | RIGHT_PAREN,
	BRACKET       = LEFT_BRACKET | RIGHT_BRACKET
};

struct Code
{
	Type type;
	union
	{
		char           c;
		char           variable_name;
		unsigned int   number;
		LabelPair*     label;
		QualifierPair* another_paren;
	};
};

class Reader
{
public:
	enum
	{
		END_OF_FILE = -1,
		TAB_SIZE = 8
	};

	Reader(Refal2* refal2, std::ostream& error_stream = std::cerr);
	~Reader();

	void push(char c);
	bool good() const;
};

class Compiler
{
public:
	static Operation* compile(Rule* rule);
};

class Executer
{
public:
	Executer();
	~Executer();

	bool run(Operation* entry_operation,
				std::ostream& output_stream, std::ostream& error_stream);
};

class Refal2
{
public:
	Refal2();
	~Refal2();

	bool loadFromFile(const std::string& filename,
						std::ostream& error_stream = std::cerr);
	bool run(std::ostream& output_stream = std::cout,
				std::ostream& error_stream = std::cerr);

private:
	bool read_file(const std::string& filename, std::ostream& error_stream);
	bool check_names(std::ostream& error_stream);

private:
	LabelsTable              labels;
	QualifiersTable          qualifiers;
	LabelsTable::value_type* entry;
	Executer                 executer;
};

} // end of namespace refal2

#endif

