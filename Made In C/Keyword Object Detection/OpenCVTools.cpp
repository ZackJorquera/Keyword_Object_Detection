#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <array>

#include "OpenCVTools.h"
#include <ppl.h>

#include "boost\uuid\uuid.hpp"
#include "boost\uuid\uuid_io.hpp"
#include "boost\uuid\uuid_generators.hpp"
#include "boost\filesystem.hpp"
 
using namespace cv;
using namespace std;

bool useValue(const pair<float, Point>& first, const pair<float, Point>& second);

int windowNum = 1;

list<Mat> LoadImages(list<string> paths, ImreadModes im)
{
	list<Mat> imgs;
	for each (string path in paths)
	{
		if (path == "")
			continue;
		
		path = TrimEnds(path, string("\"\'").c_str());

		Mat temp = imread(path, im);
		if (temp.dims != 0)
		{
			imgs.push_back(temp);
		}
		else
		{
			cout << path << " no dims." << endl;
		}
	}
	return imgs;
}

void showImages(list<Mat>* imgs)
{
	if (windowNum > 50)
		return;
	list<Mat>::iterator it = (*imgs).begin();
	for (int i = 0; i < (*imgs).size(); i++)
	{
		string name = "Window " + to_string(windowNum);
		cv::namedWindow(name, WINDOW_AUTOSIZE);
		cv::imshow(name, *it);
		++it;
		windowNum++;
	}
}

void showImage(Mat* img)
{
	if (windowNum > 50)
		return;
	string name = "Window " + to_string(windowNum);
	cv::namedWindow(name, WINDOW_AUTOSIZE);
	cv::imshow(name, *img);
	windowNum++;
}

Mat GetGradientImage(Mat grayScaleImage)
{ 
	try
	{
		Mat xGrad, yGrad, totalGrad;
		Mat absXGrad, absYGrad;

		/// Gradient X
		Sobel(grayScaleImage, xGrad, CV_16S, 1, 0, 3, 1.0, 0.0, cv::BORDER_DEFAULT);//other options include Scharr, 
		convertScaleAbs(xGrad, absXGrad);

		/// Gradient Y
		Sobel(grayScaleImage, yGrad, CV_16S, 0, 1, 3, 1.0, 0.0, cv::BORDER_DEFAULT);
		convertScaleAbs(yGrad, absYGrad);

		/// Total Gradient (where 255 is the most change in the image and 0 is no chage. so everything is preportionate)
		addWeighted(absXGrad, 0.5, absYGrad, 0.5, 0, totalGrad);

		return totalGrad;
	}
	catch(Exception ex)
	{
		cout << "GetGradientImage: " << ex.err << endl;
		return Mat();//blank
	}
}

//template<typename _Tp> inline
cv::Mat MakeMatFromRange(cv::Point start, cv::Point end, cv::Mat* image, bool ShowImages)
{
	try
	{
		if (end.x > (image->cols))
			end.x = image->cols;
		if (end.y > (image->rows))
			end.y = image->rows;
		cv::Mat Range(end.y - start.y, end.x - start.x, cv::DataType</*_Tp**/uchar>::type);

		for (int x = start.x; x < end.x; x++)
		{
			for (int y = start.y; y < end.y; y++)
			{
				try
				{
					Range.at</*_Tp**/uchar>(cv::Point(x - start.x, y - start.y)) = (*image).at</*_Tp**/uchar>(cv::Point(x, y));
				}
				catch (cv::Exception ex)
				{
					cout << ex.what() << endl;
				}
			}
		}

		if (ShowImages)
		{
			showImage(&Range);
			cv::waitKey(0);
		}

		return Range;
	}
	catch (Exception ex)
	{
		cout << "GetGradientImage: " << ex.err << endl;
		return Mat();
	}
}

void DoOneCorrelation(Mat *image, Mat *templ, float maxRot, Point p, Mat *result)
{
	int sharedPixels = 0;
	double correlation = 0;

	for (int templX = 0; templX < templ->cols; templX++)
	{
		for (int templY = 0; templY < templ->rows; templY++)
		{
			int imageX = p.x - (templ->cols / 2) + templX;
			int imageY = p.y - (templ->rows / 2) + templY;

			if (imageX >= 0 && imageX < image->cols && imageY >= 0 && imageY < image->rows)
			{
				sharedPixels++;

				int absDiff = std::abs(templ->at<uchar>(Point(templX, templY)) - image->at<uchar>(Point(imageX, imageY)));
				float thisCorrelation = (1 - (float(absDiff) / 255));
				correlation += thisCorrelation;//expand

			}
			else
				continue;
		}
	}

	float sharedPixelPresent = float(sharedPixels) / (templ->cols * templ->rows);
	correlation = ((correlation / sharedPixels) * 3 + sharedPixelPresent * 1) / 4;//add the ratio to the config file ??? dont know how yet

	uchar temp = uchar(correlation * 255);
	result->at<uchar>(p) = temp;
}

bool DoesCorrelationReachThreshold(Mat image, Mat templ, float maxRot, float threshold, Point *location, bool scaleImg2ToMatchRows)
{
	try
	{
		Mat result(image.rows, image.cols, DataType<uchar>::type);

		Concurrency::parallel_for(0, image.rows, [&](int y)//uses threading in a for loop
		{
			for (int x = 0; x < image.cols; x++)
			{
				DoOneCorrelation(&image, &templ, maxRot, Point(x, y), &result);
			}
		});

		double minVal, maxVal;
		Point minLoc, maxLoc, matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

		*location = cv::Point(maxLoc.x - (templ.cols / 2), maxLoc.y - (templ.rows / 2));

		if (maxVal/255 >= threshold)
			return true;
		else
			return false;
	}
	catch (Exception ex)
	{
		cout << "GetGradientImage: " << ex.err << endl;
		return false;
	}
}

int AddImagesAt(Mat* img1, Mat* img2, Mat* result, Point offSet, Point* mainImageOffSet, float ratio, bool cropEdgesToSquare)//addes img2 to img1 with an offset of offSet. the ratio is the present of img1, img2's present is found with 1-ratio
{
	//TODO: this throws sometimes due to being out of range
	int intersect[4];//startx starty endx endy
	int xIntersect;
	int yIntersect;
	int startX, endX, startY, endY;

	Mat resultTemp; // this is just inCase the result is the same as img1 or img2 which it is most of the time

#pragma region intersectArea

	if (offSet.x < 0)
		intersect[0] = 0;
	else
		intersect[0] = offSet.x;
	if (offSet.y < 0)
		intersect[1] = 0;
	else
		intersect[1] = offSet.y;
	if (img1->cols - 1 < img2->cols - 1 + offSet.x)
		intersect[2] = img1->cols -1;
	else
		intersect[2] = img2->cols -1 + offSet.x;
	if (img1->rows -1 < img2->rows -1 + offSet.y)
		intersect[3] = img1->rows -1;
	else
		intersect[3] = img2->rows -1 + offSet.y;

	xIntersect = intersect[2] - intersect[0] + 1;
	yIntersect = intersect[3] - intersect[1] + 1;

#pragma endregion

	if (cropEdgesToSquare || true)
	{
		resultTemp.create(img2->cols, img2->rows, cv::DataType<uchar>::type);
		if (xIntersect >= resultTemp.cols)
		{
			startX = intersect[0];
			endX = intersect[2];
		}
		else
		{
			startX = intersect[0] - ((resultTemp.cols - xIntersect) / 2);
			endX = startX + resultTemp.cols;
		}

		if (yIntersect >= resultTemp.rows)
		{
			startY = intersect[1];
			endY = intersect[3];
		}
		else
		{
			startY = intersect[1] - ((resultTemp.rows - yIntersect) / 2);
			endY = startY + resultTemp.rows;
		}
	}
	else//takes the size of img1
	{
		resultTemp.create(img1->cols, img1->rows, cv::DataType<uchar>::type);
		startX = 0;
		startY = 0;
		endX = resultTemp.cols -1;
		endY = resultTemp.rows -1;
	}
	*mainImageOffSet = Point(startX + mainImageOffSet->x, startY + mainImageOffSet->y);

	for (int x = startX; x < endX; x++)//x = img1X
	{
		for (int y = startY; y < endY; y++)//y = img1Y
		{
			int img2X = x - offSet.x;
			int img2Y = y - offSet.y;

			int resultX = x - startX;
			int resultY = y - startY;

			uchar img1Val;
			uchar img2Val;

			bool inImg1 = true;
			bool inImg2 = true;

			if (x >= 0 && x < img1->cols && y >= 0 && y < img1->rows)//if a pixel is 0 then it is a undefined pixel to counteract this everything is given one
			{
				img1Val = img1->at<uchar>(Point(x, y));
				if (img1Val == 0)
				{
					inImg1 = false;
					img1Val = 0;
				}
			}
			else
			{
				inImg1 = false;
				img1Val = 0;
			}
			if (img2X >= 0 && img2X < img2->cols && img2Y >= 0 && img2Y < img2->rows)
			{
				img2Val = img2->at<uchar>(Point(img2X, img2Y));
				if (img2Val == 0)
				{
					inImg2 = false;
					img2Val = 0;
				}
			}
			else
			{
				inImg2 = false;
				img2Val = 0;
			}

			if (!(inImg1 || inImg2))
			{
				int a = 0;
			}

			uchar temp = uchar(img1Val * ((inImg2) ? ratio : 1) + img2Val * ((inImg1) ? 1 - ratio : 1));//does this work TODO:
			resultTemp.at<uchar>(Point(resultX, resultY)) = temp;
		}
	}

	*result = resultTemp;
	return 0;
}

Mat TotalMatAddByOne(Mat image)
{
	for (int x = 0; x < image.cols; x++)
	{
		for (int y = 0; y < image.rows; y++)
		{
			uchar val = image.at<uchar>(Point(x, y));
			if (val == 255)
				continue;
			val++;
			image.at<uchar>(Point(x, y)) = val;
		}
	}
	return image;
}

Mat correlateWithConvolution(Mat src, Mat templ)
{
	try
	{
		Mat result;
		matchTemplate(src, templ, result, CV_TM_CCORR_NORMED);

		return result;
	}
	catch (cv::Exception ex)
	{
		std::cout << ex.what() << endl;
	}
}

Mat HoughFeatureAccumulater(list<feature> features, Mat src, float PresentOfBinSizeForOutput, int removeRadius, bool ShowImages)
{
	Mat H(floor(1 / double(PresentOfBinSizeForOutput)), floor(1 / double(PresentOfBinSizeForOutput)), cv::DataType<double>::type, cv::Scalar(0));
	int HRowSize = int(ceil(double(src.rows) * double(PresentOfBinSizeForOutput)));
	int HColSize = int(ceil(double(src.cols) * double(PresentOfBinSizeForOutput)));

	for each (feature thisFeature in features)
	{
		Mat result = correlateWithConvolution(src, thisFeature.grayScale);

		//*
		if (ShowImages)
		{
			showImage(&result);
			showImage(&thisFeature.grayScale);
			showImage(&src);
			waitKey(0);
		}
		//*/

		list<pair<float, Point>> pixels = FindPeaks(result, -1, 0.85, removeRadius);//0.9 Config
		
		if (pixels.size() > 0)
		{
			list<pair<float, Point>>::iterator iter = pixels.begin();
			for (int i = 0; i < 3; i++)//TODO: from config
			{
				for each (std::array<int, 4> range in thisFeature.ranges)
				{
					for (int x = range[0]; x < range[1]; x++)
					{
						for (int y = range[2]; y < range[3]; y++)
						{
							int xthing = (iter->second.x + x);
							int xPoint = floor((int(iter->second.x) + x) / HColSize);
							int yPoint = floor((int(iter->second.y + y)) / HRowSize);
							Point p(xPoint, yPoint);
							if (p.x >= 0 && p.x < H.cols && p.y >= 0 && p.y < H.rows)
							{
								H.at<double>(p)++;
								double t = H.at<double>(p);
							}
						}
					}
				}
				++iter;
			}
		}
	}
	return H;
}

bool useValue(const pair<float, Point>& first, const pair<float, Point>& second)
{
	return (first.first > second.first);
}

list<pair<float, Point>> FindPeaks(Mat image, int numOfPoints, float peakAmountThreshold, int removeRadius)
{
	list<pair<float, Point>> allPeaks;
	for (int x = 0; x < image.cols; x++)
	{
		for (int y = 0; y < image.rows; y++)
		{
			Point p;
			pair<float, Point> pixel;

			switch (image.depth())
			{
			case DataType<float>::depth:
				p = Point(x, y);
				pixel = pair<float, Point>(image.at<float>(p), p);
				if (pixel.first >= peakAmountThreshold)
					allPeaks.push_back(pixel);
				break;
			case DataType<double>::depth:
				p = Point(x, y);
				pixel = pair<float, Point>(float(image.at<double>(p)), p);
				if (pixel.first >= int(peakAmountThreshold))
					allPeaks.push_back(pixel);
				break;
			default:
				p = Point(x, y);
				pixel = pair<float, Point>(0, p);
				break;
			}
		}
	}

	list<pair<float, Point>> topPeaks;

	if (allPeaks.size() > 0)
	{
		allPeaks.sort(useValue);

		list<Point> bannedPoints;

		if (numOfPoints == -1)
			return allPeaks;

		list<pair<float, Point>>::iterator iter = allPeaks.begin();
		for (int i = 0; topPeaks.size() < numOfPoints; i ++)
		{
			if (i >= allPeaks.size())
				break;

			bool allowed = true;
			for each (Point bannedPoint in bannedPoints)
			{
				if (bannedPoint == iter->second)
				{
					allowed = false;
					break;
				}
			}

			if (allowed)
				topPeaks.push_back(*iter);
			else
			{
				++iter;
				continue;
			}

			//int removeRadius = 31;
			for (int surroundingX = -removeRadius; surroundingX <= removeRadius; surroundingX++)//TODO: config
			{
				for (int surroundingY = -removeRadius; surroundingY <= removeRadius; surroundingY++)
				{
					bannedPoints.push_back(Point(iter->second.x + surroundingX, iter->second.y + surroundingY));
				}
			}

			++iter;
		}
	}
	return topPeaks;
}

Mat DrawPointOnImage(Mat src, Point p, Scalar color, int size)
{
	for (int x = p.x - size / 2; x <= p.x + size / 2; x++)
	{
		for (int y = p.y - size / 2; y <= p.y + size / 2; y++)
		{
			if (x >= 0 && x < src.cols && y >= 0 && y < src.rows)
			{
				Point3_<uchar>* pixel = src.ptr<Point3_<uchar> >(y, x);
				pixel->x = color[2];//B
				pixel->y = color[1];//G
				pixel->z = color[0];//R
			}
		}
	}
	return src;
}