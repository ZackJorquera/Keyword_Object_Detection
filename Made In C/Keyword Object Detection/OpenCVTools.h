#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/traits.hpp"
#include <list>

#include "Tools.h"

#pragma once

//All functions that are acsesable to call from the rest of the code
std::list<cv::Mat> LoadImages(std::list<std::string>, cv::ImreadModes);
void showImages(std::list<cv::Mat>*);
void showImage(cv::Mat* img);
cv::Mat GetGradientImage(cv::Mat);

//template<typename _Tp>
cv::Mat MakeMatFromRange(cv::Point start, cv::Point end, cv::Mat* image, bool ShowImages);

bool DoesCorrelationReachThreshold(cv::Mat image, cv::Mat templ, float maxRot, float threshold, cv::Point *location, bool scaleImg2ToMatchRows);

int AddImagesAt(cv::Mat* img1, cv::Mat* img2, cv::Mat* result, cv::Point offSet, cv::Point* mainImageOffSet, float ratio, bool cropEdgesToSquare);//addes img2 to img1 with an offset of offSet. the ratio is the present of img1, img2's present is found with 1-ratio

cv::Mat TotalMatAddByOne(cv::Mat image);

cv::Mat correlateWithConvolution(cv::Mat src, cv::Mat templ);

cv::Mat HoughFeatureAccumulater(std::list<feature> features, cv::Mat src, float PresentOfImageForAccumulaterBinSizeForOutput, int removeRadius, bool ShowImages);

std::list<std::pair<float, cv::Point>> FindPeaks(cv::Mat image, int numOfPoints, float peakAmountThreshold, int removeRadius);

cv::Mat DrawPointOnImage(cv::Mat src, cv::Point p, cv::Scalar color, int size);
