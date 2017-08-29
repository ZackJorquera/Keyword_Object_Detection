#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "OpenCVTools.h"
 
using namespace cv;
using namespace std;

list<Mat>* LoadImages(list<string> paths)
{
	list<Mat>* imgs = new list<Mat>();
	for each (string path in paths)
	{
		if (path == "")
			continue;
		(*imgs).push_front(imread(path, IMREAD_COLOR));
	}
	return imgs;
}

void showImages(list<Mat> imgs)
{
	list<Mat>::iterator it = imgs.begin();
	for (int i = 0; i < (imgs).size(); i++)
	{
		string name = "Window " + to_string(i + 1);
		cv::namedWindow(name, WINDOW_AUTOSIZE);
		cv::imshow(name, *it);
		++it;
	}
}