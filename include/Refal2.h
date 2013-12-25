#ifndef _REFAL2__H_
#define _REFAL2__H_

#include <iostream>

using std::istream;
using std::ostream;

class Refal2
{
private:
	Refal2(const Refal2&);
	Refal2 &operator=(const Refal2&);
public:
	Refal2();
	Refal2(istream&, ostream&);
	~Refal2();
	
	bool Load(istream&, ostream&);
	void Run(istream&, ostream&) const;
private:
	bool status;
public:
	bool Status() const
	{
		return status;
	}
};

#endif
