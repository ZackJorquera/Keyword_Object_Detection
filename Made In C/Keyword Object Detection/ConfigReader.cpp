#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <list>

#include <fstream>

#include "ConfigReader.h"

#pragma once

std::list<std::string> ConfigVals;
std::string ConfigVals2[ConfigVarsIDCount];
std::string fileName = "config.cfg";
ConfigVarsID configVars;

void MakeConfigFile()
{
	const char configData[] = //must be in the same order as the ConfigVarsID
		"<MaxInitialFeaturesTrainer,0>10</MaxInitialFeaturesTrainer>\n"
		"<PercentOfImageWidthIsStepSizeTrainer,1>10</PercentOfImageWidthIsStepSizeTrainer>\n"
		"<FeatureComplexityThresholdTrainer,2>0.3</FeatureComplexityThresholdTrainer>\n"
		"<MaxStepSizeForFeatureTrainer,3>3</MaxStepSizeForFeatureTrainer>\n"
		"<MinStepSizeForFeatureTrainer,4>2</MinStepSizeForFeatureTrainer>\n"
		"<ImageCorralationThresholdTrainer,5>0.85</ImageCorralationThresholdTrainer>\n"
		"<CrossImageCorralationThresholdTrainer,6>0.75</CrossImageCorralationThresholdTrainer>\n"
		"<KeywordFolderPath,7>.\\KeywordImages\\</KeywordFolderPath>\n"
		"<BinSizePercentOfImageDetector,8>0.002</BinSizePercentOfImageDetector>\n"
		"<NumOfObjectsToFindPerObjectDetector,9>10</NumOfObjectsToFindPerObjectDetector>\n"
		"<NumOfPeaksToBeObjectDetector,10>10</NumOfPeaksToBeObjectDetector>\n"
		"<RadiusSurroundingPeaksToRemoveDetector,11>31</RadiusSurroundingPeaksToRemoveDetector>\n"
		;

	std::ofstream myfile;
	myfile.open(fileName);
	myfile << configData;
	myfile.close();
}

void RemoveConfig()
{
	remove(fileName.c_str());
}

void ReadConfig()
{
	try
	{
		struct stat buffer;
		if (stat(fileName.c_str(), &buffer) != 0)//if file does not exists
			MakeConfigFile();

		std::ifstream myfile(fileName, std::ifstream::in);
		while (!myfile.eof())
		{
			std::string s;
			myfile >> s;
			while (s.size() > 0 && s[s.size() - 1] != '>')
			{
				std::string tempString;
				if (myfile.eof())
					break;
				myfile >> tempString;
				s += tempString;
			}

			if (s == "")
				continue;

			std::list<std::string> parts = SplitString(s, "<>,");
			std::string name(*parts.begin());
			std::stringstream snum(*++parts.begin());
			int num;
			snum >> num;
			std::string value(*++++parts.begin());
			std::string ender(*++++++parts.begin());

			if ("/" + name == ender)//if name is the enum
				ConfigVals2[num] = value;
			else
				ConfigVals2[num] = "";
		}
		myfile.close();
	}
	catch (std::exception ex)
	{
		std::cout << "Failed to read config, Retrying\nErr: " << ex.what() << std::endl;
		RemoveConfig();
		MakeConfigFile();
		ReadConfig();
	}
}

std::stringstream GetConfigVarsFromID(int id)
{
	std::stringstream ss(ConfigVals2[id]);
	return ss;
}
