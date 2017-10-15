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
#include "Trainer.h"//just for the feature model
#include "OpenCVTools.h"
#include "ConfigReader.h"

using namespace std;
using namespace cv;

list<string> GetObjectsToFind();
list<pair<string, string>> GetFilePathsFromObjectNames(list<string> objects);
list<string> SplitString(string text, char spliter);
list<feature> CreateFeaturesFromFolderPath(string folderPath);
Mat MakeFoundImage(Mat src, float PresentOfBinSizeForOutput, list<pair<float, Point>> peaks);

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
		float PresentOfBinSizeForOutput = 0.002f;//TODO: from config
		Mat accumulatedDataForObject = HoughFeatureAccumulater(thisObjectFeatures, grayImage, PresentOfBinSizeForOutput);

		/*
		Mat showImage = accumulatedDataForObject;
		normalize(showImage, showImage, 0, 1, NORM_MINMAX, -1, Mat());
		showImage(showImage);
		waitKey(0);
		//*/

		list<pair<float, Point>> peaks = FindPeaks(accumulatedDataForObject, 3, 5);//TODO: config

		colorImage = MakeFoundImage(colorImage, PresentOfBinSizeForOutput, peaks);

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
				objectNameAndPath.first = *--(SplitString(dirIter->path().string(),'\\').end());
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

list<string> SplitString(string text, char spliter)
{
	stringstream textstream(text);
	string segment;
	list<string> strings;

	while (getline(textstream, segment, spliter))
		strings.push_back(segment);

	return strings;
}

list<feature> CreateFeaturesFromFolderPath(string folderPath)
{
	list<feature> l;

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

				fileName.erase(--------fileName.end(), fileName.end());//--------fileName.end() is the last 4

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
				l.push_back(thisFeature);
			}
		}
	}
	return l;
}

Mat MakeFoundImage(Mat src, float PresentOfBinSizeForOutput, list<pair<float, Point>> peaks)
{
	int HRowSize = ceil(double(src.rows) * double(PresentOfBinSizeForOutput));
	int HColSize = ceil(double(src.cols) * double(PresentOfBinSizeForOutput));

	for each (pair<float, Point> peak in peaks)
	{
		DrawPointOnImage(src, Point(peak.second.x * HColSize, peak.second.y * HRowSize), Scalar(0, 255, 0), 7);
	}

	return src;
}