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
		rating = float(totalEdgepixels) / (gradientImage.cols * gradientImage.rows);
	}

	bool operator==(const feature& feature1) const//this allows me to use the == operator and what is shown below is what it does
	{
		return (feature1.grayScale.data == grayScale.data && feature1.rating == rating);
	}
};

std::list<std::string> SplitString(std::string text, std::string spliters);

std::string TrimEnds(std::string text, std::string chars);