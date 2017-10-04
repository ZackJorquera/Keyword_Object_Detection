#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <list>

#include <fstream>

#include "ConfigReader.h"

#pragma once

std::list<std::string> ConfigVals;
std::string fileName = "config.cfg";
ConfigVarsID configVars;

void MakeConfigFile()
{
	const char configData[] = //must be in the same order as the ConfigVarsID
		"10-i\n"
		"10-f\n"
		"0.3-f\n"
		"3-i\n"
		"2-i\n"
		"0.85-f\n"
		"0.75-f\n"
		"C:\\KeywordImages\\-s\n"
		;

	std::ofstream myfile;
	myfile.open(fileName);
	myfile << configData;
	myfile.close();
}

void ReadConfig()
{
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) != 0)//if file exists
		MakeConfigFile();

	std::ifstream myfile(fileName, std::ifstream::in);
	while (!myfile.eof())
	{
		std::string s;
		myfile >> s;
		if (s != "")
			ConfigVals.push_back(s);
	}
	myfile.close();
}

std::stringstream GetConfigVarsFromID(int id)
{
	if (ConfigVals.size() <= id)
		ReadConfig();

	std::list<std::string>::iterator iter = ConfigVals.begin();
	for (int i = 0; i < id; i++)
		++iter;

	std::string s = *iter;
	s.erase(----s.end(), s.end());
	std::stringstream ss(s);

	return ss;
}
