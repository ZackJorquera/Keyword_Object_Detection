#include <list>
#include "opencv2/highgui/highgui.hpp"

#include "Tools.h"

#pragma once

int Train(std::string);
std::list<feature> GetMostImportantPartsOfImage(cv::Mat *grayImage, int maxFeatures, float xStepSizePersentOfImage, float maxRot, float rotStep, float thresholdPresent, int maxfeatureSizeInSteps, int minfeatureSizeInSteps);
