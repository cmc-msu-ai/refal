#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Scanner.h"

using namespace std;

int main(int argc, const char *argv[])
{
	try
	{
		if(argc != 2)
			throw string("using: refal2 program.ref");
	
		ifstream program(argv[1]);
		if(!program.is_open())
			throw string("can't open file: ").append(argv[1]);
			
		Scanner scanner;
		while(!program.eof())
		{
			if(scanner << program.get())
			{
				Lexem lexem(Lexem::STRING, string());
				scanner >> lexem;
				cout << lexem.GetText();
			}
		}
		if(scanner << Scanner::END_OF_FILE)
		{
			Lexem lexem(Lexem::STRING, string());
			scanner >> lexem;
			cout << lexem.GetText() << endl;
		}
		
		program.close();
	}
	catch(const string &e)
	{
		cout << e << endl;
	}
	catch(...)
	{
		cout << "Unknown error..." << endl;
	}
	
	return 0;
}
