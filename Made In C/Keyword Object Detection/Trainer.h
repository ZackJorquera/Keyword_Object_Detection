#include <list>
#include "opencv2/highgui/highgui.hpp"

#pragma once

struct feature
{
	cv::Mat featureGrayScale;
	cv::Mat featureGradientImage;
	std::list<std::array<int, 4>> ranges;//startx, start y, endx , endy
	float rating;

	void GetRating()
	{
		int totalEdgepixels = 0;
		for (int x = 0; x < featureGradientImage.cols; x++)
		{
			for (int y = 0; y < featureGradientImage.rows; y++)
			{
				if (featureGradientImage.at<uchar>(cv::Point(x, y)) >= 128)
					totalEdgepixels++;
			}
		}
		rating = float(totalEdgepixels) / (featureGradientImage.cols * featureGradientImage.rows) * 100;
	}
};

int Train(std::string);
std::list<feature> GetMostImportantPartsOfImage(cv::Mat *grayImage, int maxFeatures, float xStepSizePersentOfImage, float maxRot, float rotStep, float thresholdPresent, int maxfeatureSizeInSteps, int minfeatureSizeInSteps);//(cv::Mat *grayImage, int maxFeatures = 20, float xStepSizePersentOfImage = 1, float maxRot = 1, float rotStep = 0.5f, float thresholdPresent = 75, int maxfeatureSizeInSteps, int minfeatureSizeInSteps);