#pragma once

#include "Common.h"
#include <vector>

namespace Refal2 {

class CCompiler {
public:
	CCompiler();
	~CCompiler();

	void Reset();

	void Compile(CRule* function);

private:
	void compile_left_part(CLink* first, CLink* last);
	void compile_right_part(CLink* first);

	CCompiler(const CCompiler&);
	CCompiler& operator=(const CCompiler&);

	void process_holes();
	void element_match();

	struct CHole {
		CHole(CLink* _begin, CLink* _end): begin(_begin), end(_end) {}

		CLink* begin;
		CLink* end;
	};

	typedef std::vector<CHole> THoles;
	typedef std::vector<THoles> TTuples;

	THoles current_holes;
	TTuples tuples_stack;
	THoles::iterator current_hole;
};

} // end of namespace refal2
