#include "CharSet.h"
#include <bitset>

typedef std::bitset<256> bs;

CharSet::CharSet()
	: bs()
{
}
CharSet::CharSet(const CharSet &a)
	: bs(a)
{
}
CharSet::CharSet(unsigned char a)
{
		set(int(a));
}
CharSet::CharSet(unsigned char a, unsigned char b)
{
	for(int i = a; i < int(b); ++i)
		set(i);
}
CharSet::CharSet(const char *s)
{
	for(; *s != '\0'; ++s)
	{
		unsigned char c = *s;
		set(int(c));
	}
}
CharSet::~CharSet()
{
}

CharSet &CharSet::operator=(const CharSet &a)
{
	bs::operator=(a);
	return *this;
}
CharSet CharSet::operator+(const CharSet &a) const
{
	CharSet r;
	for(int i = 0; i < 256; i++)
		if(test(i) || a.test(i))
			r.set(i);
	return r;		
}
CharSet &CharSet::operator+=(const CharSet &a)
{
	for(int i = 0; i < 256; i++)
		if(a.test(i))
			set(i);
	return *this;		
}
CharSet CharSet::operator-(const CharSet &a) const
{
	CharSet r;
	for(int i = 0; i < 256; i++)
		if(test(i) && !a.test(i))
			r.set(i);
	return r;		
}
CharSet &CharSet::operator-=(const CharSet &a)
{
	for(int i = 0; i < 256; i++)
		if(a.test(i))
			reset(i);
	return *this;		
}
