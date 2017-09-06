#include <list>
#include "opencv2/highgui/highgui.hpp"

#pragma once

struct feature
{
	cv::Mat grayScale;
	cv::Mat gradientImage;
	std::list<std::array<int, 4>> ranges;//startx, start y, endx , endy
	float rating;

	void GetRating()
	{
		int totalEdgepixels = 0;
		for (int x = 0; x < gradientImage.cols; x++)
		{
			for (int y = 0; y < gradientImage.rows; y++)
			{
				if (gradientImage.at<uchar>(cv::Point(x, y)) >= 128)
					totalEdgepixels++;
			}
		}
		rating = float(totalEdgepixels) / (gradientImage.cols * gradientImage.rows) * 100;
	}

	bool operator==(const feature& feature1) const//this allows me to use the == operator and what is shown below is what it does
	{
		return (feature1.grayScale.data == grayScale.data && feature1.rating == rating);
	}
};

int Train(std::string);
std::list<feature> GetMostImportantPartsOfImage(cv::Mat *grayImage, int maxFeatures, float xStepSizePersentOfImage, float maxRot, float rotStep, float thresholdPresent, int maxfeatureSizeInSteps, int minfeatureSizeInSteps);
