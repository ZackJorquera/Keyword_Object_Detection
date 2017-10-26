#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <array>

#include "boost\uuid\uuid.hpp"
#include "boost\uuid\uuid_io.hpp"
#include "boost\uuid\uuid_generators.hpp"
#include "boost\filesystem.hpp"

#include "Tools.h"

using namespace std;

list<string> SplitString(string text, string spliters)
{
	stringstream textstream(text);
	string segment("");
	list<string> strings;

	std::vector<char> spliterChars(spliters.c_str(), spliters.c_str() + spliters.size() + 1);

	string::iterator sIter = text.begin();
	for (int i = 0; i < text.size(); i++)
	{
		bool isSpliter = false;
		for each (char spliter in spliters)
		{
			if (spliter == *sIter)
				isSpliter = true;
		}

		if (isSpliter)
		{
			if(segment != "")
				strings.push_back(segment);
			segment = "";
		}
		else
			segment += *sIter;

		++sIter;
	}

	if (segment != "")
		strings.push_back(segment);

	return strings;
}