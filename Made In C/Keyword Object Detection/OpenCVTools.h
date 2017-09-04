#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/traits.hpp"
#include <list>

#pragma once

//All functions that are acsesable to call from the rest of the code
std::list<cv::Mat>* LoadImages(std::list<std::string>, cv::ImreadModes);
void showImages(std::list<cv::Mat>*);
void showImage(cv::Mat* img);
cv::Mat GetGradientImage(cv::Mat);

//template<typename _Tp>
cv::Mat MakeMatFromRange(cv::Point start, cv::Point end, cv::Mat* image);

bool DoesCorralationReachThreshold(cv::Mat image, cv::Mat templ, float maxRot, float threshold, cv::Point *location);

int AddImagesAt(cv::Mat* img1, cv::Mat* img2, cv::Mat* result, cv::Point offSet, float ratio, bool cropEdgesToSquare);