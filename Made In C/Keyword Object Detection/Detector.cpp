#include <iostream>
#include <list>
#include <array>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "boost\uuid\uuid.hpp"
#include "boost\uuid\uuid_io.hpp"
#include "boost\uuid\uuid_generators.hpp"
#include "boost\filesystem.hpp"

#include "Detector.h"
#include "OpenCVTools.h"
#include "ConfigReader.h"

using namespace std;
using namespace cv;

list<string> GetObjectsToFind();
list<pair<string, string>> GetFilePathsFromObjectNames(list<string> objects);
list<feature> CreateFeaturesFromFolderPath(string folderPath);
Mat DrawOnImage(Mat src, float PresentOfBinSizeForOutput, list<pair<float, Point>> peaks);

bool ShowWorkImages;

int Find(string arg)
{
	ReadConfig();

	string imageFile;
	Mat grayImage;
	Mat colorImage;

	list<string> thingsToFind;

	cout << "Info: Finds where different pre-trained objects are in an image.\n";

	cout << "Image file location: ";

	cin >> imageFile;

	ShowWorkImages = (arg == "-s");

	thingsToFind = GetObjectsToFind();

	list<string> tempList;
	tempList.push_back(imageFile);
	grayImage = LoadImages(tempList, IMREAD_GRAYSCALE).front();
	colorImage = LoadImages(tempList, IMREAD_COLOR).front();

	list<pair<string,string>> thingsToFindWithFiles = GetFilePathsFromObjectNames(thingsToFind);

	list<pair<string, string>>::iterator objectIter = thingsToFindWithFiles.begin();
	for (int i = 0; i < thingsToFindWithFiles.size(); i++)
	{
		string name = objectIter->first;
		list<feature> thisObjectFeatures = CreateFeaturesFromFolderPath(objectIter->second);
		float PresentOfBinSizeForOutput;
		GetConfigVarsFromID(BinSizePercentOfImageDetector) >> PresentOfBinSizeForOutput;
		int NumOfPeaksToBeObject;
		GetConfigVarsFromID(NumOfPeaksToBeObjectDetector) >> NumOfPeaksToBeObject;
		int numPoints;
		GetConfigVarsFromID(NumOfObjectsToFindPerObjectDetector) >> numPoints;
		int standerdRemoveRadius;
		GetConfigVarsFromID(RadiusSurroundingPeaksToRemoveDetector) >> standerdRemoveRadius;

		Mat accumulatedDataForObject = HoughFeatureAccumulater(thisObjectFeatures, grayImage, PresentOfBinSizeForOutput, standerdRemoveRadius, ShowWorkImages);

		//*
		if (ShowWorkImages)
		{
			Mat theShowImage;
			accumulatedDataForObject.copyTo(theShowImage);
			normalize(theShowImage, theShowImage, 0, 1, NORM_MINMAX, -1, Mat());
			showImage(&theShowImage);
			waitKey(0);
		}
		//*/

		list<pair<float, Point>> peaks = FindPeaks(accumulatedDataForObject, numPoints, NumOfPeaksToBeObject, standerdRemoveRadius);//TODO: config

		colorImage = DrawOnImage(colorImage, PresentOfBinSizeForOutput, peaks);

		++objectIter;
	}

	showImage(&colorImage);
	waitKey(0);

	return 0;
}

list<string> GetObjectsToFind()
{
	list<string> objectNames;

	cout << "How many things to look for? If you want to do all just write \"all\".  ";

	int num;
	string s;
	cin >> s;
	if (s == "all")
	{
		objectNames.push_back("all");
		return objectNames;
	}
	std::stringstream stringIntConverter(s);
	stringIntConverter >> num;

	for (int i = 0; i < num; i++)
	{
		string object = "";
		cout << "Object " << (i + 1) << ": ";
		while (object == "")
		{
			object = "";
			getline(cin, object);
			if (object != "")
				objectNames.push_back(object);
		}
	}
	return objectNames;
}

list<pair<string, string>> GetFilePathsFromObjectNames(list<string> objects)
{
	list<pair<string,string>> objectFilePaths;
	
	string keywordFolderPath;
	GetConfigVarsFromID(KeywordFolderPath) >> keywordFolderPath;

	cout << "Finding:" << endl;

	list<string>::iterator iter = objects.begin();
	if (*iter == "all")
	{
		boost::filesystem::directory_iterator end;
		boost::filesystem::path dir(keywordFolderPath);
		boost::filesystem::directory_iterator dirIter(dir);

		for (dirIter; dirIter != end; ++dirIter)
		{
			if (boost::filesystem::is_directory(dirIter->status()))
			{
				pair<string, string> objectNameAndPath;
				objectNameAndPath.first = *--(SplitString(dirIter->path().string(),"\\").end());
				objectNameAndPath.second = (dirIter->path().string() + "\\");
				objectFilePaths.push_back(objectNameAndPath);
				cout << objectNameAndPath.first << endl;
			}
		}
		return objectFilePaths;
	}

	for (int i = 0; i < objects.size(); i++)
	{
		boost::filesystem::path dir(keywordFolderPath + (*iter));
		if (boost::filesystem::exists(dir))
		{
			pair<string, string> objectNameAndPath;
			objectNameAndPath.first = *iter;
			objectNameAndPath.second = (keywordFolderPath + (*iter) + "\\");
			objectFilePaths.push_back(objectNameAndPath);
			cout << objectNameAndPath.first << endl;
		}

		++iter;
	}
	
	return objectFilePaths;
}

list<feature> CreateFeaturesFromFolderPath(string folderPath)
{
	list<feature> thisList;

	boost::filesystem::directory_iterator end;
	boost::filesystem::path dir(folderPath);
	boost::filesystem::directory_iterator dirIter(dir);
	for (dirIter; dirIter != end; ++dirIter)
	{
		if (boost::filesystem::is_regular_file(dirIter->status()))
		{
			string fileName = dirIter->path().string();

			if (dirIter->path().extension().string() == ".jpg")
			{
				feature thisFeature;

				list<string> imagePathList;
				imagePathList.push_back(fileName);
				thisFeature.grayScale = *LoadImages(imagePathList, IMREAD_GRAYSCALE).begin();

				fileName.erase(--------fileName.end(), fileName.end());//--------fileName.end() is the last 4 so it erases ".jpg"

				ifstream myfile(fileName + ".vctrinf", std::ifstream::in);
				while (!myfile.eof())
				{
					string segment;
					myfile >> segment;
					if (segment.size() > 0)
					{
						if (*segment.begin() == '[')
						{
							array<int, 4> range;
							for (int i = 0; i < 4; i++)
							{
								myfile >> segment;

								if (*segment.begin() == ']')
									break;
								stringstream segmentAsInt(segment);
								segmentAsInt >> range[i];

							}
							thisFeature.ranges.push_back(range);
						}
					}
				}
				myfile.close();
				thisList.push_back(thisFeature);
			}
		}
	}
	return thisList;
}

Mat DrawOnImage(Mat src, float PresentOfBinSizeForOutput, list<pair<float, Point>> peaks)
{
	int HRowSize = ceil(double(src.rows) * double(PresentOfBinSizeForOutput));
	int HColSize = ceil(double(src.cols) * double(PresentOfBinSizeForOutput));

	for each (pair<float, Point> peak in peaks)
	{
		DrawPointOnImage(src, Point(peak.second.x * HColSize, peak.second.y * HRowSize), Scalar(0, 255, 0), 7);
	}

	return src;
}