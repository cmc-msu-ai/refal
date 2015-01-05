#include "Reader.h"

#include <ctype.h>
#include <string>

namespace Refal2
{

CReader::CReader(CCommonReader& _common, std::ostream& _error_stream):
	common(_common),
	error_stream(_error_stream),
	status(S_ok),
	line(1),
	offset(1),
	lexem_number(0),
	right_paren_was_last(false),
	current_function(_common.labels.end())
{
	common.entry = 0;
	common.labels.clear();
	common.qualifiers.clear();
}

CReader::~CReader()
{
}

void CReader::CRuleParser::Reset()
{
	state = S_begin;
	variable_type = '\0';
	variable_qualifier = 0;
	paren_balance_counter = 0;
	while( !stack.empty() ) {
		stack.pop();
	}
}

void CReader::Step(char c)
{
	process_char(c);
	
	if( c == '\n' ) {
		++line;
		scanner.offset = 1;
	} else if( c == '\t' ) {
		while( scanner.offset % CScanner::TAB_SIZE ) {
			++scanner.offset;
		}
	} else {
		++scanner.offset;
	}
}

void CReader::End()
{
	process_end();
}

std::string CReader::lower(const std::string& s)
{
	std::string r(s);
	for(std::string::iterator i = r.begin(); i != r.end(); ++i) {
		*i = ::tolower(*i);
	}
	return r;
}

// ----------------------------------------------------------------------------

bool CReader::is_qualifier_name_available()
{
	return ( common.qualifiers.find(lower(current_name)) ==
		common.qualifiers.end() );
}

void CReader::reset_qualifier()
{
	qualifier_builder.Reset();
	right_paren_was_last = false;
}

void CReader::save_qualifier()
{
	CQualifier* q = create_qualifier();

	if( q != 0 ) {
		std::cout << "set_q: " << lower(current_name) << std::endl;

		common.qualifiers.insert(std::make_pair(lower(current_name), q));
	} else {
		/* TODO: error */
		//error(RE_cant_create_qualifier);
	}

	reset_qualifier();
}

CQualifier* CReader::create_qualifier()
{
	if( !qualifier_builder.IsNegative() ) {
		if( !right_paren_was_last ) {
			qualifier_builder.AddNegative();
		}
		CQualifier* q = new CQualifier();
		qualifier_builder.Export(q);
		qualifier_builder.Reset();
		return q;
	} else {
		/* TODO: ERROR */
		//on_error(RE_unclosed_left_paren_in_qualifier);
	}
	return 0;
}

bool CReader::process_qualifier(Lexem lexem)
{
	right_paren_was_last = false;
	switch( lexem )
	{
		case LEXEM_LABEL:
			{
				TLabels::iterator i = common.labels.find(lower(lexem_buffer));
				if( i != common.labels.end() ) {
					qualifier_builder.AddLabel(&(*i));
				} else {
					/* TODO: ERROR */
					return false;
				}
			}
			break;
		case LEXEM_NUMBER:
			qualifier_builder.AddNumber(lexem_number);
			break;
		case LEXEM_STRING:
			for( std::string::const_iterator i = lexem_buffer.begin();
				i != lexem_buffer.end(); ++ i )
			{
				qualifier_builder.AddChar(*i);
			}
			break;
		case LEXEM_LEFT_PAREN:
			if( !qualifier_builder.IsNegative() ) {
				qualifier_builder.AddNegative();
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case LEXEM_RIGHT_PAREN:
			if( qualifier_builder.IsNegative() ) {
				qualifier_builder.AddNegative();
				right_paren_was_last = true;
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case LEXEM_IDENT:
			for( std::string::const_iterator i = lexem_buffer.begin();
				i != lexem_buffer.end(); ++ i )
			{
				switch( *i ) {
					case 'w' :
					case 'W' :
						qualifier_builder.AddW();
						break;
					case 's' :
					case 'S' :
						qualifier_builder.AddS();
						break;
					case 'b' :
					case 'B' :
						qualifier_builder.AddB();
						break;
					case 'f' :
					case 'F' :
						qualifier_builder.AddF();
						break;
					case 'n' :
					case 'N' :
						qualifier_builder.AddN();
						break;
					case 'o' :
					case 'O' :
						qualifier_builder.AddO();
						break;
					case 'd' :
					case 'D' :
						qualifier_builder.AddD();
						break;
					case 'l' :
					case 'L' :
						qualifier_builder.AddL();
						break;
					default:
						/* TODO: ERROR */
						return false;
						break;
				}
			}
			break;
		case LEXEM_QUALIFIER:
			{
				TQualifiers::const_iterator i =
					common.qualifiers.find(lower(lexem_buffer));
				if( i != common.qualifiers.end() ) {
					qualifier_builder.AddQualifier(*(i->second));
				} else {
					/* TODO: ERROR */
					return false;
				}
			}
			break;
		case LEXEM_BLANK:
			break;
		default:
			/* TODO: ERROR */
			return false;
			break;
	}
	
	return true;
}

// ----------------------------------------------------------------------------

void CReader::set_entry_function(const std::string& name)
{
	if( common.entry == 0 ) {
		std::cout << "set_entry: " << lower(name) << std::endl;

		std::pair<TLabels::iterator, bool> i = common.labels.insert(
			std::make_pair(lower(name), CFunction()));
		
		common.entry = &(*(i.first));
	} else {
		/* TODO: ERROR redeclaration of entry */
	}
}

void CReader::set_empty_function(const std::string& name)
{	
	std::pair<TLabels::iterator, bool> i = common.labels.insert(
		std::make_pair(lower(name), CFunction(common.EMPTY_FUNCTION)));

	if( i.second ) {
		std::cout << "set_empty: " << lower(name) << std::endl;
	} else {
		/* TODO: redeclaration of empty function */
	}
}

void CReader::set_extrn_function(const std::string& name,
								 const std::string& oldname)
{	
	std::pair<TLabels::iterator, bool> i = common.labels.insert(
		std::make_pair(lower(name), CFunction(common.EXTRN_FUNCTION)));

	if( i.second ) {
		std::cout << "set_extrn: " << lower(name) << std::endl;
		
		/* TODO: get pointer to extrn */		
	} else {
		/* TODO: ERROR */
	}
}

// ----------------------------------------------------------------------------

void CReader::reset_current_function()
{
	std::cout << "reset_current_function();\n";

	if( current_function != common.labels.end() ) {
		function_builder.Get( &(current_function->second.rule) );
	}

	rule_parser.Reset();
	function_builder.Reset();
	current_function = common.labels.end();
}

bool CReader::set_current_function()
{
	reset_current_function();
	
	std::pair<TLabels::iterator, bool> r = common.labels.insert(
		std::make_pair(lower(current_name), CFunction()));

	if( true || r.second ) { // TODO: or ...
		current_function = r.first;

		std::cout << "set_current_function(" << lower(current_name) << ");\n";
	} else {
		/* TODO: ERROR */
		return false;
	}

	return true;
}


bool CReader::finalize_left_part()
{   
	std::cout << "finalize_left_part{}\n";

	function_builder.EndOfLeft();
	
	return true;
}

bool CReader::finalize_rule()
{
	std::cout << "finalize_rule()\n";

	function_builder.EndOfRule();
	
	return true;
}

bool CReader::append_string()
{
	std::cout << "append_string{" << lexem_buffer << "}\n";

	for( std::string::const_iterator i = lexem_buffer.begin();
		i != lexem_buffer.end(); ++i )
	{
		function_builder.Char(*i);
	}
	
	return true;
}

bool CReader::append_label()
{
	std::cout << "append_label{" << lower(lexem_buffer) << "}\n";
	
	std::pair<TLabels::iterator, bool> i = common.labels.insert(
		std::make_pair(lower(lexem_buffer), CFunction()));
	
	function_builder.Label( &(*(i.first)) );
		
	return true;
}

bool CReader::append_number()
{
	std::cout << "append_number{" << lexem_number << "}\n";

	function_builder.Number(lexem_number);
	
	return true;
}

bool CReader::append_left_paren()
{
	std::cout << "append_left_paren{}\n";

	rule_parser.stack.push(function_builder.LeftParen());

	return true;
}

bool CReader::append_right_paren()
{
	std::cout << "append_right_paren{}\n";

	CLink* link = 0;
	
	if( !rule_parser.stack.empty() ) {
		link = rule_parser.stack.top();
		rule_parser.stack.pop();
	}

	if( link != 0 && link->IsLeftParen() ) {
		CUnitLink* right_paren = function_builder.RightParen();
		right_paren->PairedParen() = link->Unit();
		link->PairedParen() = right_paren;
	} else {
		/* TODO: ERROR */
		return false;
	}

	return true;
}

bool CReader::append_variable(TVariableType type, TVariableName name,
	CQualifier* variable_qualifier)
{
	std::cout << "append_variable{" << type << name << "}\n";

	function_builder.Variable(type, name, variable_qualifier);

	return true;
}

bool CReader::set_variable_qualifier()
{
	std::cout << "set_variable_qualifier{}\n";

	return true;
}

bool CReader::append_left_bracket()
{
	std::cout << "append_left_bracket{}\n";

	rule_parser.stack.push(function_builder.LeftBracket());

	return true;
}

bool CReader::append_right_bracket()
{
	std::cout << "append_right_bracket{}\n";
		
	if( !rule_parser.stack.empty()
		&& rule_parser.stack.top()->IsLeftBracket() )
	{
		rule_parser.stack.pop();
	} else {
		/* TODO: ERROR */
		return false;
	}

	return true;
}

bool CReader::process_function(Lexem lexem)
{
	if( current_function == common.labels.end() ) {
		/* TODO: error unexpected rule */
		return false;
	}
	
	switch( rule_parser.state ) {
		case CRuleParser::S_begin :
			if( lexem == LEXEM_BLANK ) {
			} else if( lexem == LEXEM_IDENT ) {
				if( lexem_buffer == "L" || lexem_buffer == "l" ) {
					rule_parser.state = CRuleParser::S_begin_order;
				} else if( lexem_buffer == "R" || lexem_buffer == "r" ) {
					rule_parser.state = CRuleParser::S_begin_order;
					function_builder.RightDirection(); // action
				} else {
					rule_parser.state = CRuleParser::S_begin_left_part;
					return process_function(lexem);
				}
			} else {
				rule_parser.state = CRuleParser::S_begin_left_part;
				return process_function(lexem);
			}
			break;
		case CRuleParser::S_begin_order :
			if( lexem == LEXEM_BLANK ) {
				rule_parser.state = CRuleParser::S_begin_left_part;
			} else {
				/* TODO: ERROR expected BLANK after MATCHING DIRECTION */
				return false;
			}
			break;
		case CRuleParser::S_begin_left_part :
			if( lexem == LEXEM_BLANK ) {
			} else if( lexem == LEXEM_EQUAL ) {
				rule_parser.state = CRuleParser::S_begin_right_part;
				return finalize_left_part(); // action
			} else if( lexem == LEXEM_STRING ) {
				return append_string(); // action
			} else if( lexem == LEXEM_LABEL ) {
				return append_label(); // action
			} else if( lexem == LEXEM_NUMBER ) {
				return append_number(); // action
			} else if( lexem == LEXEM_LEFT_PAREN ) {
				return append_left_paren(); // action
			} else if( lexem == LEXEM_RIGHT_PAREN ) {
				return append_right_paren(); // action
			} else if( lexem == LEXEM_IDENT ) {
				char c = ::tolower(lexem_buffer[0]);
				if( c == 's' || c == 'w' || c == 'v' || c == 'e' ) {
					rule_parser.state = CRuleParser::S_begin_variable;
					lexem_buffer.erase(0, 1);
					++offset;
					rule_parser.variable_type = c; // action
					rule_parser.variable_qualifier = 0; // action
				
					if( !lexem_buffer.empty() ) {
						return process_function(lexem);
					}
				} else {
					/* TODO: ERROR incorrect variable type */
					return false;
				}
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case CRuleParser::S_begin_variable :
			if( lexem == LEXEM_LEFT_PAREN ) {
				rule_parser.state = CRuleParser::S_begin_qualifier;
				rule_parser.paren_balance_counter = 1;
				qualifier_builder.Reset(); // action
			} else if( lexem == LEXEM_QUALIFIER ) {
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				qualifier_builder.Reset(); // action
				if( !process_qualifier(LEXEM_QUALIFIER) ) { // action
					/* TODO: ERROR incorrect qualifier */
					return false;
				}
			} else {
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				return process_function(lexem);
			}
			break;
		case CRuleParser::S_begin_variable_qualifier :
			if( lexem == LEXEM_IDENT ) {
				char name = lexem_buffer[0];
				if( isalnum(name) ) {
					rule_parser.state = CRuleParser::S_begin_left_part;
					lexem_buffer.erase(0, 1);
					++offset;
					
					if( !append_variable(rule_parser.variable_type, name,
						rule_parser.variable_qualifier) )
					{
						return false;
					}
				
					if( !lexem_buffer.empty() ) {
						return process_function(lexem);
					}
				} else {
					/* TODO: ERROR */
					return false;
				}
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case CRuleParser::S_begin_qualifier :
			if( lexem == LEXEM_RIGHT_PAREN &&
				rule_parser.paren_balance_counter == 1 )
			{
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				return set_variable_qualifier();
			} else {
				if( lexem == LEXEM_LEFT_PAREN ) {
					++rule_parser.paren_balance_counter;
				} else if( lexem == LEXEM_RIGHT_PAREN ) {
					--rule_parser.paren_balance_counter;
				}
				
				if( !process_qualifier(lexem) ) {
					/* TODO: ERROR incorrect qualifier */
					return false;
				}
			}
			break;
		case CRuleParser::S_begin_right_part :
			switch( lexem ) {
				case LEXEM_BLANK :
					break;
				case LEXEM_NEWLINE :
					rule_parser.state = CRuleParser::S_begin;
					if( rule_parser.stack.empty() ) {
						return finalize_rule();
					} else {
						/* TODO: ERROR */
					}
					break;
				case LEXEM_STRING :
					return append_string();
					break;
				case LEXEM_LABEL :
					return append_label();
					break;
				case LEXEM_NUMBER :
					return append_number();
					break;
				case LEXEM_LEFT_PAREN :
					return append_left_paren();
					break;
				case LEXEM_RIGHT_PAREN :
					return append_right_paren();
					break;
				case LEXEM_LEFT_BRACKET :
					rule_parser.state =
						CRuleParser::S_begin_left_bracket;
					return append_left_bracket();
					break;
				case LEXEM_RIGHT_BRACKET :
					return append_right_bracket();
					break;
				case LEXEM_IDENT :
					{
						char t = tolower(lexem_buffer[0]);
						char n = lexem_buffer[1];
			
						if( t == 's' || t == 'w' || t == 'v' || t == 'e' ) {
							if( isalnum(n) ) {
								lexem_buffer.erase(0, 2);
								offset += 2;

								if( !append_variable(t, n) ) {
									return false;
								}
					
								if( !lexem_buffer.empty() ) {
									return process_function(lexem);
								}
							} else {
								/* TODO: ERROR */
								return false;
							}
						} else {
							/* TODO: ERROR incorrect variable type */
							return false;
						}
					}
					break;
				default :
					/* TODO: ERROR */
					return false;
			}
			break;
		case CRuleParser::S_begin_left_bracket :
			rule_parser.state = CRuleParser::S_begin_right_part;
			if( lexem == LEXEM_IDENT ) {
				return append_label();
			} else {
				return process_function(lexem);
			}
			break;
		default :
			break;
	}

	return true;
}

#if 0
bool CReader::process_function(Lexem lexem)
{
	if( current_function == common.labels.end() ) {
		/* TODO: error unexpected rule */
		return false;
	}
	
	switch( rule_parser.state ) {
		case CRuleParser::S_begin :
			if( lexem == LEXEM_BLANK ) {
			} else if( lexem == LEXEM_IDENT ) {
				if( lexem_buffer == "L" || lexem_buffer == "l" ) {
					rule_parser.state = CRuleParser::S_begin_order;
				} else if( lexem_buffer == "R" || lexem_buffer == "r" ) {
					rule_parser.state = CRuleParser::S_begin_order;
					function_builder.RightDirection(); // action
				} else {
					rule_parser.state = CRuleParser::S_begin_left_part;
					return process_function(lexem);
				}
			} else {
				rule_parser.state = CRuleParser::S_begin_left_part;
				return process_function(lexem);
			}
			break;
		case CRuleParser::S_begin_order :
			if( lexem == LEXEM_BLANK ) {
				rule_parser.state = CRuleParser::S_begin_left_part;
			} else {
				/* TODO: ERROR expected BLANK after MATCHING DIRECTION */
				return false;
			}
			break;
		case CRuleParser::S_begin_left_part :
			if( lexem == LEXEM_BLANK ) {
			} else if( lexem == LEXEM_EQUAL ) {
				rule_parser.state = CRuleParser::S_begin_right_part;
				return finalize_left_part(); // action
			} else if( lexem == LEXEM_STRING ) {
				return append_string(); // action
			} else if( lexem == LEXEM_LABEL ) {
				return append_label(); // action
			} else if( lexem == LEXEM_NUMBER ) {
				return append_number(); // action
			} else if( lexem == LEXEM_LEFT_PAREN ) {
				return append_left_paren(); // action
			} else if( lexem == LEXEM_RIGHT_PAREN ) {
				return append_right_paren(); // action
			} else if( lexem == LEXEM_IDENT ) {
				char c = ::tolower(lexem_buffer[0]);
				if( c == 's' || c == 'w' || c == 'v' || c == 'e' ) {
					rule_parser.state = CRuleParser::S_begin_variable;
					lexem_buffer.erase(0, 1);
					++offset;
					rule_parser.variable_type = c; // action
					rule_parser.variable_qualifier = 0; // action
				
					if( !lexem_buffer.empty() ) {
						return process_function(lexem);
					}
				} else {
					/* TODO: ERROR incorrect variable type */
					return false;
				}
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case CRuleParser::S_begin_variable :
			if( lexem == LEXEM_LEFT_PAREN ) {
				rule_parser.state = CRuleParser::S_begin_qualifier;
				rule_parser.paren_balance_counter = 1;
				qualifier_builder.Reset(); // action
			} else if( lexem == LEXEM_QUALIFIER ) {
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				qualifier_builder.Reset(); // action
				if( !process_qualifier(LEXEM_QUALIFIER) ) { // action
					/* TODO: ERROR incorrect qualifier */
					return false;
				}
			} else {
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				return process_function(lexem);
			}
			break;
		case CRuleParser::S_begin_variable_qualifier :
			if( lexem == LEXEM_IDENT ) {
				char name = lexem_buffer[0];
				if( isalnum(name) ) {
					rule_parser.state = CRuleParser::S_begin_left_part;
					lexem_buffer.erase(0, 1);
					++offset;
					
					if( !append_variable(rule_parser.variable_type, name,
						rule_parser.variable_qualifier) )
					{
						return false;
					}
				
					if( !lexem_buffer.empty() ) {
						return process_function(lexem);
					}
				} else {
					/* TODO: ERROR */
					return false;
				}
			} else {
				/* TODO: ERROR */
				return false;
			}
			break;
		case CRuleParser::S_begin_qualifier :
			if( lexem == LEXEM_RIGHT_PAREN &&
				rule_parser.paren_balance_counter == 1 )
			{
				rule_parser.state =
					CRuleParser::S_begin_variable_qualifier;
				return set_variable_qualifier();
			} else {
				if( lexem == LEXEM_LEFT_PAREN ) {
					++rule_parser.paren_balance_counter;
				} else if( lexem == LEXEM_RIGHT_PAREN ) {
					--rule_parser.paren_balance_counter;
				}
				
				if( !process_qualifier(lexem) ) {
					/* TODO: ERROR incorrect qualifier */
					return false;
				}
			}
			break;
		case CRuleParser::S_begin_right_part :
			switch( lexem ) {
				case LEXEM_BLANK :
					break;
				case LEXEM_NEWLINE :
					rule_parser.state = CRuleParser::S_begin;
					if( rule_parser.stack.empty() ) {
						return finalize_rule();
					} else {
						/* TODO: ERROR */
					}
					break;
				case LEXEM_STRING :
					return append_string();
					break;
				case LEXEM_LABEL :
					return append_label();
					break;
				case LEXEM_NUMBER :
					return append_number();
					break;
				case LEXEM_LEFT_PAREN :
					return append_left_paren();
					break;
				case LEXEM_RIGHT_PAREN :
					return append_right_paren();
					break;
				case LEXEM_LEFT_BRACKET :
					rule_parser.state =
						CRuleParser::S_begin_left_bracket;
					return append_left_bracket();
					break;
				case LEXEM_RIGHT_BRACKET :
					return append_right_bracket();
					break;
				case LEXEM_IDENT :
					{
						char t = tolower(lexem_buffer[0]);
						char n = lexem_buffer[1];
			
						if( t == 's' || t == 'w' || t == 'v' || t == 'e' ) {
							if( isalnum(n) ) {
								lexem_buffer.erase(0, 2);
								offset += 2;

								if( !append_variable(t, n) ) {
									return false;
								}
					
								if( !lexem_buffer.empty() ) {
									return process_function(lexem);
								}
							} else {
								/* TODO: ERROR */
								return false;
							}
						} else {
							/* TODO: ERROR incorrect variable type */
							return false;
						}
					}
					break;
				default :
					/* TODO: ERROR */
					return false;
			}
			break;
		case CRuleParser::S_begin_left_bracket :
			rule_parser.state = CRuleParser::S_begin_right_part;
			if( lexem == LEXEM_IDENT ) {
				return append_label();
			} else {
				return process_function(lexem);
			}
			break;
		default :
			break;
	}

	return true;
}
#endif

// ----------------------------------------------------------------------------

void CReader::errorl(Lexem lexem)
{
}

void CReader::process_lexem(Lexem lexem)
{
	switch( parser.state )
	{
		case CParser::BEGIN:
			if( lexem == LEXEM_IDENT ) {
				parser.state = CParser::BEGIN_IDENT;
				current_name = lexem_buffer; // action
				reset_current_function(); // action
			} else if( lexem == LEXEM_BLANK ) {
				parser.state = CParser::BEGIN_BLANK;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				reset_current_function(); // action
				/* TODO: ERROR */
			}
			break;
		case CParser::ONLY_NEWLINE:
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::WAIT_NEWLINE:
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
			}
			break;
		case CParser::BEGIN_IDENT:
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
				set_current_function(); // action
			} else if( lexem == LEXEM_BLANK ) {
				parser.state = CParser::BEGIN_IDENT_BLANK;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_IDENT_BLANK:
			if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "start" ) {
				parser.state = CParser::ONLY_NEWLINE;
				/* TODO: WARNING */
			} else if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "s" ) {
				parser.state = CParser::BEGIN_IDENT_BLANK_S;
				parser.tmp = offset;
			} else {
				if( set_current_function() ) { // action
					parser.state = CParser::S_process_rule;
				} else {
					parser.state = CParser::WAIT_NEWLINE;
				}
				process_lexem(lexem);
			}
			break;
		case CParser::BEGIN_IDENT_BLANK_S:
			if( lexem == LEXEM_BLANK ) {
				if( is_qualifier_name_available() ) { // action
					reset_qualifier();
					parser.state = CParser::S_process_qualifier;
				} else {
					parser.state = CParser::WAIT_NEWLINE;
				}
			} else {
				if( set_current_function() ) { // action
					parser.state = CParser::S_process_rule;
					
					std::string tmp_buffer;
					lexem_buffer.swap(tmp_buffer);
					lexem_buffer = 'S';
			
					std::swap(offset, parser.tmp);

					process_lexem(LEXEM_IDENT);
			
					lexem_buffer.swap(tmp_buffer);
					offset = parser.tmp;
				} else {
					parser.state = CParser::WAIT_NEWLINE;
				}
				process_lexem(lexem);
			}
			break;
		case CParser::BEGIN_BLANK :
			if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "end" ) {
				parser.state = CParser::ONLY_NEWLINE;
				reset_current_function(); // action
				/* TODO: WARNING */
			} else if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "extrn" ) {
				parser.state = CParser::BEGIN_EXTRN;
				reset_current_function(); // action
			} else if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "empty" ) {
				parser.state = CParser::BEGIN_EMPTY;
				reset_current_function(); // action
			} else if( lexem == LEXEM_IDENT && lower(lexem_buffer) == "entry" ) {
				parser.state = CParser::BEGIN_ENTRY;
				reset_current_function(); // action
			} else {
				parser.state = CParser::S_process_rule;
				process_lexem(lexem);
			}
			break;
		/* entry */
		case CParser::BEGIN_ENTRY :
			if( lexem == LEXEM_BLANK ) {
				parser.state = CParser::BEGIN_ENTRY_BLANK;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_ENTRY_BLANK :
			if( lexem == LEXEM_IDENT ) {
				parser.state = CParser::ONLY_NEWLINE;
				set_entry_function(lexem_buffer); // action
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		/* end of entry */
		/* empty */
		case CParser::BEGIN_EMPTY :
			if( lexem == LEXEM_BLANK ) {
				parser.state = CParser::BEGIN_EMPTY_COMMA;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EMPTY_COMMA :
			if( lexem == LEXEM_IDENT ) {
				parser.state = CParser::BEGIN_EMPTY_IDENT;
				set_empty_function(lexem_buffer); // action
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EMPTY_IDENT :
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
			} else if( lexem == LEXEM_COMMA ) {
				parser.state = CParser::BEGIN_EMPTY_COMMA;
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		/* end of empty */
		/* extrn */
		case CParser::BEGIN_EXTRN :
			if( lexem == LEXEM_BLANK ) {
				parser.state = CParser::BEGIN_EXTRN_COMMA;
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EXTRN_COMMA :
			if( lexem == LEXEM_IDENT ) {
				parser.state = CParser::BEGIN_EXTRN_IDENT;
				current_name = lexem_buffer; // action
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EXTRN_IDENT :
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
				set_extrn_function(current_name); // action
			} else if( lexem == LEXEM_COMMA ) {
				parser.state = CParser::BEGIN_EXTRN_COMMA;
				set_extrn_function(current_name); // action
			} else if( lexem == LEXEM_LEFT_PAREN ) {
				parser.state = CParser::BEGIN_EXTRN_LEFT_PAREN;
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EXTRN_LEFT_PAREN :
			if( lexem == LEXEM_IDENT ) {
				parser.state = CParser::BEGIN_EXTRN_INNER_IDENT;
				set_extrn_function(current_name, lexem_buffer); // action
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EXTRN_INNER_IDENT :
			if( lexem == LEXEM_RIGHT_PAREN ) {
				parser.state = CParser::BEGIN_EXTRN_ONLY_COMMA;
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		case CParser::BEGIN_EXTRN_ONLY_COMMA :
			if( lexem == LEXEM_COMMA ) {
				parser.state = CParser::BEGIN_EXTRN_COMMA;
			} else if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
			} else if( lexem == LEXEM_BLANK ) {
			} else {
				parser.state = CParser::WAIT_NEWLINE;
				/* TODO: ERROR */
			}
			break;
		/* end of extrn */
		case CParser::S_process_qualifier:
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
				save_qualifier(); // action
			} else if( !process_qualifier(lexem) ) { // action
				parser.state = CParser::WAIT_NEWLINE;
				reset_qualifier(); // action
			}
			break;
		case CParser::S_process_rule :
			if( !process_function(lexem) ) { // action
				parser.state = CParser::WAIT_NEWLINE;
				reset_current_function(); // action
			}
			if( lexem == LEXEM_NEWLINE ) {
				parser.state = CParser::BEGIN;
			}
			break;
		default :
			break;
	}
}

// ----------------------------------------------------------------------------

void CReader::errorc(TLexicalErrorCodes code, char c) {
	error_stream << line << ":" << scanner.offset << ": ERROR: ";
	switch( code ) {
		case LEC_unexpected_control_sequence:
			error_stream << "Unexpected '\\x" <<
				std::hex << int(c) << "' control sequence!\n";
			break;
		case LEC_symbol_after_plus:
			error_stream << "Unexpected '" << c << "' character after '+'. "
				"Only spaces or comments can be followed after plus.\n";
			break;
		case LEC_unexpected_character:
			error_stream << "Unexpected '" << c << "' character\n";
			break;
		case LEC_illegal_character_in_label_or_number_begin:
			error_stream << "Illegal '" << c <<
				"' character in the begining of label or number. "
				"Label can start only with alpha or '_'.\n";
			break;
		case LEC_illegal_character_in_label:
			error_stream << "Illegal '" << c << "' character in label. "
				"Label can consists only of alpha, digit or '_'.\n";
			break;
		case LEC_illegal_character_in_number:
			error_stream << "Illegal '" << c <<
				"' character in number. Number can consists only of digit.\n";
			break;
		case LEC_illegal_character_in_qualifier_name_begin:
			error_stream << "Illegal '" << c <<
				"' character in the begining of qualifier's name. "
				"Qualifier's name can start only with alpha or '_'.\n";
			break;
		case LEC_illegal_character_in_qualifier_name:
			error_stream << "Illegal '" << c <<
				"' character in the qualifier's name. "
				"Qualifier's name consists only of alpha, digit or '_'.\n";
			break;
		case LEC_illegal_character_in_string_after_slash:
			error_stream << "Illegal '" << c <<
				"' character in string constant after '\\'. "
				"Use c-like string constant.\n";
			break;
		case LEC_illegal_character_in_string_in_hexadecimal:
			error_stream << "Illegal '" << c <<
				"' character in string constant after '\\x' (hexadecimal). "
				"Use c-like string constant.\n";
			break;
		case LEC_trying_append_null_byte_to_string:
			error_stream << "Null byte '\\0' in string constant!\n";
			break;
		case LEC_illegal_character_in_string_in_octal:
			error_stream << "Unexpected '" << c <<
				"' character in string constant after '\\0' (octal). "
				"Use c-like string constant.\n";
			break;
		case LEC_unclosed_string_constant_at_the_end_of_file:
			error_stream << "Unclosed string constant at the end of file!\n";
			break;
		case LEC_unclosed_string_constant:
			error_stream << "Unclosed string constant at the end of line!\n";
			break;
		case LEC_unclosed_label_or_number_at_the_end_of_file:
			error_stream << "Unclosed label or number at the end of file!\n";
			break;
		case LEC_unclosed_label_or_number:
			error_stream << "Unclosed label or number at the end of line!\n";
			break;
		case LEC_unclosed_label_at_the_end_of_file:
			error_stream << "Unclosed label at the end of file!\n";
			break;
		case LEC_unclosed_label:
			error_stream << "Unclosed label at the end of line!\n";
			break;
		case LEC_unclosed_number_at_the_end_of_file:
			error_stream << "Unclosed number at the end of file!\n";
			break;
		case LEC_unclosed_number:
			error_stream << "Unclosed number at the end of line!\n";
			break;
		case LEC_unclosed_qualifier_at_the_end_of_file:
			error_stream << "Unclosed qualifier's name at the end of file!\n";
			break;
		case LEC_unclosed_qualifier:
			error_stream << "Unclosed qualifier's name at the end of line!\n";
			break;
		case LEC_unexpected_end_of_file:
			error_stream << "Unexpected end of file!\n";
			break;
		default:
			error_stream << "Unknown...!\n";
			/* assert */
			break;
	}
	on_error();
}

void CReader::process_char(char c)
{
	if( c == '\r' ) {
	} else if( c == '\x7f' || ( c <= '\x1f' && c != '\n' && c != '\t' ) ) {
		errorc(LEC_unexpected_control_sequence, c); // error
	} else {
		switch(scanner.state) {
			case CScanner::BEGIN_OF_LINE :
				if( c == ' ' || c == '\t' ) {
					scanner.state = CScanner::BEGIN_BLANK;
				} else if( c == '\n' ) {
				} else if( c == '+' ) {
					scanner.state = CScanner::BEGIN_PLUS;
				} else if( c == '*' ) {
					scanner.state = CScanner::BEGIN_COMMENT;
				} else {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_char(c);
				}
				break;
			case CScanner::BEGIN_BLANK :
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					scanner.state = CScanner::BEGIN_COMMENT;
				} else if( c == '+' ) {
					scanner.state = CScanner::BEGIN_PLUS;
				} else if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
				} else {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					offset = scanner.offset;
					process_lexem(LEXEM_BLANK);
					process_char(c);
				}
				break;
			case CScanner::BEGIN_COMMENT :
				if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
				}
				break;
			case CScanner::BEGIN_PLUS :
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					scanner.state = CScanner::BEGIN_COMMENT_AFTER_PLUS;
				} else if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
				} else {
					errorc(LEC_symbol_after_plus, c); // error
				}
				break;
			case CScanner::BEGIN_COMMENT_AFTER_PLUS :
				if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
				}
				break;
			case CScanner::NOT_BEGIN_OF_LINE :
				if( c == '+' ) {
					scanner.state = CScanner::NOT_BEGIN_PLUS;
				} else if( c == '*' ) {
					scanner.state = CScanner::NOT_BEGIN_COMMENT;
				} else if( c == ' ' || c == '\t' ) {
					scanner.state = CScanner::NOT_BEGIN_BLANK;
				} else if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
					offset = scanner.offset;
					process_lexem(LEXEM_NEWLINE);
				} else if( c == '\'' ) {
					scanner.state = CScanner::S;
					lexem_buffer.clear();
					offset = scanner.offset;
				} else if( c == '/' ) {
					scanner.state = CScanner::BEGIN_LABEL_OR_NUMBER;
					offset = scanner.offset;
				} else if( c == ':' ) {
					scanner.state = CScanner::BEGIN_QUALIFIER;
				} else if( c == '(' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_LEFT_PAREN);
				} else if( c == ')' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_RIGHT_PAREN);
				} else if( c == '<' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_LEFT_BRACKET);
				} else if( c == '>' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_RIGHT_BRACKET);
				} else if( c == '=' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_EQUAL);
				} else if( c == ',' ) {
					offset = scanner.offset;
					process_lexem(LEXEM_COMMA);
				} else if( isalnum(c) || c == '_' ) {
					scanner.state = CScanner::IDENTIFICATOR;
					lexem_buffer = c;
					offset = scanner.offset;
				} else {
					errorc(LEC_unexpected_character, c); // error
				}
				break;
			case CScanner::NOT_BEGIN_BLANK :
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					scanner.state = CScanner::NOT_BEGIN_COMMENT;
				} else if( c == '+' ) {
					scanner.state = CScanner::NOT_BEGIN_PLUS;
				} else if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
					offset = scanner.offset;
					process_lexem(LEXEM_NEWLINE);
				} else {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					offset = scanner.offset;
					process_lexem(LEXEM_BLANK);
					process_char(c);
				}
				break;
			case CScanner::NOT_BEGIN_COMMENT :
				if( c == '\n' ) {
					scanner.state = CScanner::BEGIN_OF_LINE;
					offset = scanner.offset;
					process_lexem(LEXEM_NEWLINE);
				}
				break;
			case CScanner::NOT_BEGIN_PLUS :
				if( c == ' ' || c == '\t' ) {
				} else if( c == '*' ) {
					scanner.state = CScanner::NOT_BEGIN_COMMENT_AFTER_PLUS;
				} else if( c == '\n' ) {
					scanner.state = CScanner::NOT_BEGIN_BLANK;
				} else {
					errorc(LEC_symbol_after_plus, c); // error
				}
				break;
			case CScanner::NOT_BEGIN_COMMENT_AFTER_PLUS :
				if( c == '\n' ) {
					scanner.state = CScanner::NOT_BEGIN_BLANK;
				}
				break;
			case CScanner::BEGIN_LABEL_OR_NUMBER :
				if( isalpha(c) || c == '_' ) {
					scanner.state = CScanner::NOT_BEGIN_LABEL;
					lexem_buffer = c;
				} else if( isdigit(c) ) {
					scanner.state = CScanner::NOT_BEGIN_NUMBER;
					lexem_number = c - '0';
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_label_or_number); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
				} else {
					// error
					errorc(LEC_illegal_character_in_label_or_number_begin, c);
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_char(c);
				}
				break;
			case CScanner::NOT_BEGIN_LABEL :
				if( isalnum(c) || c == '_' ) {
					lexem_buffer += c;
				} else if( c == '/' ) {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_LABEL);
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_label); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_LABEL);
				} else {
					errorc(LEC_illegal_character_in_label, c); // error
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_LABEL);
					process_char(c);
				}
				break;
			case CScanner::NOT_BEGIN_NUMBER :
				if( isdigit(c) ) {
					lexem_number *= 10;
					lexem_number += c - '0';
				} else if( c == '/' ) {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_NUMBER);
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_number); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_NUMBER);
				} else {
					errorc(LEC_illegal_character_in_number, c); // error
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_NUMBER);
					process_char(c);
				}
				break;
			case CScanner::IDENTIFICATOR :
				if( isalnum(c) || c == '_' ) {
					lexem_buffer += c;
				} else {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_IDENT);
					process_char(c);
				}
				break;
			case CScanner::BEGIN_QUALIFIER :
				if( isalpha(c) || c == '_' ) {
					scanner.state = CScanner::NOT_BEGIN_QUALIFIER;
					lexem_buffer = c;
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_qualifier); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
				} else {
					// error
					errorc(LEC_illegal_character_in_qualifier_name_begin, c);
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_char(c);
				}
				break;
			case CScanner::NOT_BEGIN_QUALIFIER :
				if( isalnum(c) || c == '_' ) {
					lexem_buffer += c;
				} else if( c == ':' ) {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_QUALIFIER);
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_qualifier); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_QUALIFIER);
				} else {
					errorc(LEC_illegal_character_in_qualifier_name, c); // error
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_QUALIFIER);
					process_char(c);
				}
				break;
			case CScanner::S :
				if( c == '\'' ) {
					scanner.state = CScanner::SA;
				} else if( c == '\\' ) {
					scanner.state = CScanner::SB;
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_string_constant); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_STRING);
				} else {
					lexem_buffer += c;
				}
				break;
			case CScanner::SA :
				if( c == '\'' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\'';
				} else {
					scanner.state = CScanner::NOT_BEGIN_OF_LINE;
					process_lexem(LEXEM_STRING);
					process_char(c);
				}
				break;
			case CScanner::SB :
				if( c == 'a' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\a';
				} else if( c == 'b' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\b';
				} else if( c == 'f' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\f';
				} else if( c == 'n' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\n';
				} else if( c == 'r' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\r';
				} else if( c == 't' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\t';
				} else if( c == 'v' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\v';
				} else if( c == '\\' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\\';
				} else if( c == '\'' ) {
					scanner.state = CScanner::S;
					lexem_buffer += '\'';
				} else if( c == 'x' ) {
					scanner.state = CScanner::SX;
				} else if( c == '0' ) {
					scanner.state = CScanner::S0;
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_string_constant); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_STRING);
				} else {
					// error
					errorc(LEC_illegal_character_in_string_after_slash, c);
				}
				break;
			case CScanner::SX :
				if( isdigit(c) ) {
					scanner.state = CScanner::SXA;
					scanner.tmp = c - '0';
				} else if( c >= 'a' && c <= 'f' ) {
					scanner.state = CScanner::SXA;
					scanner.tmp = c - 'a' + 10;
				} else if( c >= 'A' && c <= 'F' ) {
					scanner.state = CScanner::SXA;
					scanner.tmp = c - 'A' + 10;
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_string_constant); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_STRING);
				} else {
					// error
					errorc(LEC_illegal_character_in_string_in_hexadecimal, c);
				}
				break;
			case CScanner::SXA :
				if( isdigit(c) ) {
					scanner.tmp *= 16;
					scanner.tmp += c - '0';
				} else if( c >= 'a' && c <= 'f' ) {
					scanner.tmp *= 16;
					scanner.tmp += c - 'a' + 10;
				} else if( c >= 'A' && c <= 'F' ) {
					scanner.tmp *= 16;
					scanner.tmp += c - 'A' + 10;
				} else {
					if( scanner.tmp != 0 ) {
						scanner.state = CScanner::S;
						lexem_buffer += static_cast<char>(scanner.tmp % 256);
					} else {
						errorc(LEC_trying_append_null_byte_to_string); // error
					}
					process_char(c);
				}
				break;
			case CScanner::S0 :
				if( c >= '0' && c <= '7' ) {
					scanner.state = CScanner::S0A;
					scanner.tmp = c - '0';
				} else if( c == '\n' ) {
					errorc(LEC_unclosed_string_constant); // error
					scanner.state = CScanner::BEGIN_OF_LINE;
					process_lexem(LEXEM_STRING);
				} else {
					// error
					errorc(LEC_illegal_character_in_string_in_octal, c);
				}
				break;
			case CScanner::S0A :
				if( c >= '0' && c <= '7' ) {
					scanner.tmp *= 8;
					scanner.tmp += c - '0';
				} else {
					if( scanner.tmp != 0 ) {
						scanner.state = CScanner::S;
						lexem_buffer += static_cast<char>(scanner.tmp % 256);
					} else {
						errorc(LEC_trying_append_null_byte_to_string); // error
					}
					process_char(c);
				}
				break;
			default :
				break;
		}
	}
}

void CReader::process_end()
{
	switch(scanner.state) {
		case CScanner::BEGIN_OF_LINE:
		case CScanner::BEGIN_BLANK:
		case CScanner::BEGIN_COMMENT:
			reset_current_function(); // action
			break;

		case CScanner::NOT_BEGIN_OF_LINE:
		case CScanner::NOT_BEGIN_BLANK:
		case CScanner::NOT_BEGIN_COMMENT:
			offset = scanner.offset;
			process_lexem(LEXEM_NEWLINE);
			reset_current_function(); // action
			break;

		case CScanner::IDENTIFICATOR:
			scanner.state = CScanner::NOT_BEGIN_OF_LINE;
			process_lexem(LEXEM_IDENT);
			process_end();
			reset_current_function(); // action
			break;

		case CScanner::SA:
			scanner.state = CScanner::NOT_BEGIN_OF_LINE;
			process_lexem(LEXEM_STRING);
			process_end();
			reset_current_function(); // action
			break;
		
		case CScanner::S:
		case CScanner::SB:
		case CScanner::SX:
		case CScanner::SXA:
		case CScanner::S0:
		case CScanner::S0A:
			errorc(LEC_unclosed_string_constant_at_the_end_of_file); // error
			break;

		case CScanner::BEGIN_PLUS:
		case CScanner::NOT_BEGIN_PLUS:
		case CScanner::BEGIN_COMMENT_AFTER_PLUS:
		case CScanner::NOT_BEGIN_COMMENT_AFTER_PLUS:
			errorc(LEC_unexpected_end_of_file); // error
			break;

		case CScanner::BEGIN_LABEL_OR_NUMBER:
			errorc(LEC_unclosed_label_or_number_at_the_end_of_file); // error
			break;

		case CScanner::NOT_BEGIN_LABEL:
			errorc(LEC_unclosed_label_at_the_end_of_file); // error
			break;

		case CScanner::NOT_BEGIN_NUMBER:
			errorc(LEC_unclosed_number_at_the_end_of_file); // error
			break;

		case CScanner::BEGIN_QUALIFIER:
		case CScanner::NOT_BEGIN_QUALIFIER:
			errorc(LEC_unclosed_qualifier_at_the_end_of_file); // error
			break;

		default:
			break;
	}
}

} // end of namespace Refal2
