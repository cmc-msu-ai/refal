#ifndef _CHARSET__H_
#define _CHARSET__H_

#include <bitset>

class CharSet : public std::bitset<256>
{
public:
	CharSet();
	CharSet(const CharSet &a);
	CharSet(char a);
	CharSet(unsigned char a, unsigned char b);
	CharSet(const char *s);
	~CharSet();

	CharSet &operator=(const CharSet &a);
	CharSet operator+(const CharSet &a) const;
	CharSet &operator+=(const CharSet &a);
	CharSet operator-(const CharSet &a) const;
	CharSet &operator-=(const CharSet &a);
};

#endif
