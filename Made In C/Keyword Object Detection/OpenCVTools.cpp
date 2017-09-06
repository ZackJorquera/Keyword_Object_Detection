#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "OpenCVTools.h"
 
using namespace cv;
using namespace std;

int windowNum = 1;

list<Mat> LoadImages(list<string> paths, ImreadModes im)
{
	list<Mat> imgs;
	for each (string path in paths)
	{
		if (path == "")
			continue;
		Mat temp = imread(path, im);
		if(temp.dims != 0)
			imgs.push_back(temp);
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
cv::Mat MakeMatFromRange(cv::Point start, cv::Point end, cv::Mat* image)
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

		//showImage(&Range);
		//cv::waitKey(0);

		return Range;
	}
	catch (Exception ex)
	{
		cout << "GetGradientImage: " << ex.err << endl;
		return Mat();
	}
}

bool DoesCorralationReachThreshold(Mat image, Mat templ, float maxRot, float threshold, Point *location, bool scaleImg2ToMatchRows)
{
	//Make my own so that it searches the edges better
	try
	{
		Mat result;

		matchTemplate(templ, image, result, CV_TM_CCORR_NORMED);//get a Correlation map also TODO: make my own of this that get overlap
		//normalize(result, result, 0, 1, NORM_, -1, Mat());// macks from 0 to 1

		/*
		std::list <cv::Mat> t;
		t.push_back(image);
		t.push_back(templ);
		t.push_back(result);
		showImages(&t);
		cv::waitKey(0);
		//*/

		double minVal, maxVal;
		Point minLoc, maxLoc, matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

		*location = maxLoc;

		if (maxVal > threshold)
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

int AddImagesAt(Mat* img1, Mat* img2, Mat* result, Point offSet, float ratio, bool cropEdgesToSquare)//addes img2 to img1 with an offset of offSet. the ratio is the present of img1, img2's present is found with 1-ratio
{
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

	for (int x = startX; x < endX + 1; x++)//x = img1X
	{
		for (int y = startY; y < endY + 1; y++)//y = img1Y
		{
			int img2X = x - offSet.x;
			int img2Y = y - offSet.y;

			int resultX = x - startX;
			int resultY = y - startY;

			uchar img1Val;
			uchar img2Val;

			if (x >= 0 && x < img1->cols && y >= 0 && y < img1->rows)
				img1Val = img1->at<uchar>(Point(x, y));
			else
				img1Val = -1;
			if (img2X >= 0 && img2X < img2->cols && img2Y >= 0 && img2Y < img2->rows)
				img2Val = img2->at<uchar>(Point(img2X, img2Y));
			else
				img2Val = -1;
			
			uchar temp = uchar(img1Val * ((img2Val != -1) ? ratio : 1) + img2Val * ((img1Val != -1) ? 1 - ratio : 1));//does this work TODO:
			resultTemp.at<uchar>(Point(resultX, resultY)) = temp;
		}
	}

	*result = resultTemp;
	return 0;
}
