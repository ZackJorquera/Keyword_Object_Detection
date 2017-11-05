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

string TrimEnds(string text, string chars)
{
	if (text == "")
		return "";

	string TrimText = text;
	string::iterator frontIter = TrimText.begin();
	string::iterator backIter = --TrimText.end();
	int NewStartVal;
	int NewEndVal;
	const size_t valsInChars = chars.size();

	for (NewStartVal = 0; NewStartVal < TrimText.size(); NewStartVal++)
	{
		bool isInChars = false;
		for (int i = 0; i < valsInChars; i ++)
		{
			if (*frontIter == chars[i])
				isInChars = true;
		}
		if (!isInChars)
			break;
		++frontIter;
	}
	for (NewEndVal = TrimText.size() -1; NewEndVal >= 0; NewEndVal++)
	{
		bool isInChars = false;
		for (int i = 0; i < valsInChars; i++)
		{
			if (*backIter == chars[i])
				isInChars = true;
		}
		if (!isInChars)
			break;
		--backIter;
	}
	try
	{
		if (NewEndVal != TrimText.size() - 1)
		{
			++backIter;
			TrimText.erase(backIter, TrimText.end());
		}
		if (NewStartVal != 0)
		{
			--frontIter;
			if (NewStartVal > 1)
				TrimText.erase(TrimText.begin(), frontIter);
			else
				TrimText.erase(frontIter);
		}
	}
	catch (exception ex)
	{
		return text;
	}
	return TrimText;
}