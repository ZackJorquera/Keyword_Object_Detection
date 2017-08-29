#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/traits.hpp"
#include <list>

#pragma once

//All functions that are acsesable to call from the rest of the code
std::list<cv::Mat>* LoadImages(std::list<std::string>);
void showImages(std::list<cv::Mat>);