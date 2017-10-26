#include <list>

#include "Tools.h"

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
	BinSizePercentOfImageDetector = 8,
	NumOfObjectsToFindPerObjectDetector = 9,
	NumOfPeaksToBeObjectDetector = 10,
	RadiusSurroundingPeaksToRemoveDetector = 11,

	ConfigVarsIDCount
};

void MakeConfigFile();
void ReadConfig();
std::stringstream GetConfigVarsFromID(int id);
