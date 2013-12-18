#ifndef _REFAL2_H_
#define _REFAL2_H_

#include <iostream>
#include <string>

using std::istream;
using std::ostream;
using std::string;

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
	string text;
	bool status;

public:
	bool Status() const
	{
		return status;
	}
};

#endif
