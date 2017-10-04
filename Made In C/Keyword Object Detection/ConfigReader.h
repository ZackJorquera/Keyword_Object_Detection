#include <list>

#pragma once

enum ConfigVarsID
{
	MaxInitialFeaturesTrainer = 0,
	PercentOfImageWidthIsStepSizeTrainer = 1,
	FeatureComplexityThresholdTrainer = 2,
	MaxStepSizeForFeatureTrainer = 3,
	MinStepSizeForFeatureTrainer = 4,
	ImageCorralationThresholdTrainer = 5,
	CrossImageCorralationThresholdTrainer = 6,
	KeywordFolderPath = 7,

};

void MakeConfigFile();
void ReadConfig();
std::stringstream GetConfigVarsFromID(int id);
