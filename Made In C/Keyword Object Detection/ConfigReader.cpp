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
		"<KeywordFolderPath,7>C:\\KeywordImages\\</KeywordFolderPath>\n"
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

void ReadConfig()
{
	struct stat buffer;
	if (stat(fileName.c_str(), &buffer) != 0)//if file does not exists
		MakeConfigFile();

	std::ifstream myfile(fileName, std::ifstream::in);
	while (!myfile.eof())
	{
		std::string s;
		myfile >> s;
		
		if (s == "")
			continue;

		std::list<std::string> parts = SplitString(s, "<>, ");
		std::string name(*parts.begin());
		std::stringstream snum(*++parts.begin());
		int num;
		snum >> num;
		std::string value(*++++parts.begin());
		std::string ender(*++++++parts.begin());

		if("/" + name == ender)//if name is the enum
			ConfigVals2[num] = value;
		else
			ConfigVals2[num] = "";
	}
	myfile.close();
}

std::stringstream GetConfigVarsFromID(int id)
{
	std::stringstream ss(ConfigVals2[id]);
	return ss;
}
