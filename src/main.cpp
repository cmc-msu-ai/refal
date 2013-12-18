#include <iostream>
#include <fstream>
#include "Refal2.h"

using std::string;
using std::ifstream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, const char *argv[])
{
	try
	{
		Refal2 refal;
		if(argc == 2)
		{
			ifstream input(argv[1]);
		
			if(!input.is_open())
				throw string("can't open file: ").append(argv[1]);
			
			refal.Load(input, cerr);
		}
		else
		{
			refal.Load(cin, cerr);
		}
		
		if(!refal.Status())
		{
			return 1;
		}
		refal.Run(cin, cout);
	}
	catch(const string &e)
	{
		cerr << e << endl;
		return 1;
	}
	catch(...)
	{
		cerr << "Unknown error!" << endl;
		return 1;
	}
	
	return 0;
}
