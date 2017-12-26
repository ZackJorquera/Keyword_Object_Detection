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

#include "Trainer.h"
#include "OpenCVTools.h"
#include "ConfigReader.h"
#include "Tools.h"

using namespace std;


void GetInputImages(list<string>* imageFiles);
bool useRating(const feature& first, const feature& second);
list<feature> CorrelateImageFeatures(list<feature>* keyFeatures);
list<feature> CorrelateFeaturesCrossImage(list<list<feature>>* featuresForEachImage);
list<feature> CreateKeywords(list<list<pair<feature*, cv::Point>>> keywordLists, int thresholdOfSharedImages);
int SaveMultipleAsImgFiles(std::string folderPath, std::string folderName, std::list<feature>* images);
int SaveOneAsImgFile(std::string folderPath, std::string folderName, feature* image);

bool ShowImagesT;
bool JustShowLastImageT;

string runPath;

int Train(string arg, string path)
{
	ReadConfig();

	list<string> imageFiles;
	list<cv::Mat> grayScaleimages;
	string objectName;

	cout << "Info: the center of the image will be used as the center of the object. for the best results please crop the object to the edges of the image so that only the object is classifyed as the object." << endl;
	
	GetInputImages(&imageFiles);

	cout << "please ender the name of the object in the images: ";
	cin >> objectName;

	grayScaleimages = LoadImages(imageFiles, cv::IMREAD_GRAYSCALE);

	ShowImagesT = (arg == "-s");
	JustShowLastImageT = (arg == "-ls");
	//*
	if (ShowImagesT)//shows the "keywords"
	{
		showImages(&grayScaleimages);
		cv::waitKey(0);
	}

	int maxFeatures, maxfeatureSizeInSteps, minfeatureSizeInSteps;
	float xStepSizePersentOfImage, thresholdPresent;
	string folderPath;

	GetConfigVarsFromID(MaxInitialFeaturesTrainer) >> maxFeatures;
	GetConfigVarsFromID(MaxStepSizeForFeatureTrainer) >> maxfeatureSizeInSteps;
	GetConfigVarsFromID(MinStepSizeForFeatureTrainer) >> minfeatureSizeInSteps;
	GetConfigVarsFromID(PercentOfImageWidthIsStepSizeTrainer) >> xStepSizePersentOfImage;
	GetConfigVarsFromID(FeatureComplexityThresholdTrainer) >> thresholdPresent;
	GetConfigVarsFromID(KeywordFolderPath) >> folderPath;

	runPath = path;
	string::iterator i(runPath.end());
	for (i; (*i) != '\\' && i != runPath.begin(); --i) {}
	runPath.erase(i, runPath.end());

	list<list<feature>> keyFeaturesPerImage;

	//Gets all posable features that could be used for finding for each image
	list<cv::Mat>::iterator it = grayScaleimages.begin(); ////(*it) is the object in the list at i
	for (int i = 0; i < grayScaleimages.size(); i++)
	{
		list<feature> t = GetMostImportantPartsOfImage(&*it, maxFeatures, xStepSizePersentOfImage, 0, 0, thresholdPresent, maxfeatureSizeInSteps,minfeatureSizeInSteps);
		keyFeaturesPerImage.push_back(t);

		/*//shows the images in t
		if (ShowImages)
		{
			list<feature>::iterator it2 = t.begin(); // this is how you read through a list in c++
			for (int i = 0; i < t.size(); i++)
			{
				showImage(&it2->grayScale);
				++it2;
			}
			cv::waitKey(0);
		}
		//*/
		++it;//goes to the next object in the list, does not throw if it is moved to one that does not exist to this will work fine
	}
	
	list<list<feature>>::iterator keyFeaturesIterator = keyFeaturesPerImage.begin();//*it2
	for (int i = 0; i < keyFeaturesPerImage.size(); i++)
	{
		*keyFeaturesIterator = CorrelateImageFeatures(&*keyFeaturesIterator);
		++keyFeaturesIterator;
		//see if there are any duplicets in the list like two tires on a car and combine them this means there will be a list of vetor range things
	}
	//then add the images together (EVENYLY just halfing will not work)
	list<feature> keywords;
	if (keyFeaturesPerImage.size() > 1)
		keywords = CorrelateFeaturesCrossImage(&keyFeaturesPerImage);
	else if (keyFeaturesPerImage.size() == 1)
		keywords = *keyFeaturesPerImage.begin();

	cout << "saving Files." << endl;
	SaveMultipleAsImgFiles(folderPath, objectName, &keywords);

	//*
	if (ShowImagesT || JustShowLastImageT)
	{
		for (list<feature>::iterator iter = keywords.begin(); iter != keywords.end(); ++iter)
		{
			showImage(&iter->grayScale);
		}
		cv::waitKey(0);
	}
	//*/

	return 0;
}

list<feature> CorrelateImageFeatures(list<feature>* keyFeatures)
{
	list<list<pair<feature*, cv::Point>>> keywordLists;

	list<feature>::iterator featureIterator = keyFeatures->begin();
	for (int i = 0; i < keyFeatures->size(); i++)
	{
		list<pair<feature*, cv::Point>> keywordToBe;
		keywordToBe.push_front(pair<feature*, cv::Point>(&*featureIterator, cv::Point(0, 0)));//added the first one

		list<feature>::iterator featureIteratorFromBack = keyFeatures->end();
		--featureIteratorFromBack;//I do this becasue the iterator has an item at the end that is there to say there is no more items
		for (int j = int(keyFeatures->size()) - 1; j > i; j--)
		{
			float corralationThreshold;
			GetConfigVarsFromID(ImageCorralationThresholdTrainer) >> corralationThreshold;

			pair<feature*, cv::Point> thisFeature;
			thisFeature.first = &*featureIteratorFromBack;
			if (DoesCorrelationReachThreshold(featureIterator->grayScale, featureIteratorFromBack->grayScale, 0.0, corralationThreshold, &thisFeature.second, false))
			{
				keywordToBe.push_back(thisFeature);
			}
			--featureIteratorFromBack;
		}
		keywordLists.push_back(keywordToBe);

		++featureIterator;
	}
	return CreateKeywords(keywordLists, 0);

}

void GetInputImages(list<string>* imageFiles)
{

	cout << endl << "How many images are going to be used to train with: ";

	int num;
	cin >> num;

	for (int i = 0; i < num; i++)
	{
		string thisImage = "";
		cout << "Image File Location " << i + 1 << ": ";
		while (thisImage == "")
		{
			thisImage = "";
			getline(cin, thisImage);
			if(thisImage != "")
				imageFiles->push_back(thisImage);
		}
	}
}

list<feature> GetMostImportantPartsOfImage(
	cv::Mat *grayImage, int maxFeatures, float xStepSizePersentOfImage, 
	float maxRot, float rotStep, float thresholdPresent, int maxfeatureSizeInSteps, 
	int minfeatureSizeInSteps)
{
	using namespace cv;
	Mat gradImg;
	list<feature> Features;
	gradImg = GetGradientImage(*grayImage);

	int pixelsPerStep = int(gradImg.cols / (100 / xStepSizePersentOfImage));//pps is pixels per step

	for (int x = 0; x < int(ceil(gradImg.cols/ double(pixelsPerStep))); x++)
	{
		for (int y = 0; y < int(ceil(gradImg.rows / double(pixelsPerStep))); y++)
		{
			for (int s = minfeatureSizeInSteps; s < maxfeatureSizeInSteps; s++)
			{
				feature f;
				f.gradientImage = MakeMatFromRange(
					cv::Point(x * pixelsPerStep, y * pixelsPerStep), 
					cv::Point((x + s) * pixelsPerStep, (y + s) * pixelsPerStep), 
					&gradImg, 
					ShowImagesT);

				f.grayScale = TotalMatAddByOne(MakeMatFromRange(
					cv::Point(x * pixelsPerStep, y * pixelsPerStep), 
					cv::Point((x + s) * pixelsPerStep, (y + s) * pixelsPerStep), 
					grayImage, 
					ShowImagesT));

				f.GetRating();
				if (f.rating < thresholdPresent)
					continue;
				std::array <int, 4> range;//for c++ you have to use std::array in order to have an array in a list
				range[0] = (gradImg.cols / 2) - (x)*pixelsPerStep;
				range[1] = ((gradImg.cols + 1) / 2) - (x - 0.5f)*pixelsPerStep;
				range[2] = (gradImg.rows / 2) - (y)*pixelsPerStep;
				range[3] = ((gradImg.rows + 1) / 2) - (y - 0.5f)*pixelsPerStep;
				f.ranges.push_back(range);
																																																								//TODO: fix the issue where the images are all the same thing but diffrent sizes
				Features.push_back(f);
			}
		}
	}

	if(Features.size() < maxFeatures/2)
		Features = GetMostImportantPartsOfImage(grayImage, maxFeatures, xStepSizePersentOfImage, 0, 0, 0, maxfeatureSizeInSteps, minfeatureSizeInSteps);

	Features.sort(useRating);
	while (Features.size() > maxFeatures)
		Features.pop_back();

	return Features;
}

bool useRating(const feature& first, const feature& second)
{
	return (first.rating > second.rating);
}

list<feature> CorrelateFeaturesCrossImage(list<list<feature>>* featuresForEachImage)//TODO: test
{
	list<list<pair<feature*,cv::Point>>> keywordLists;

	list<list<feature>>::iterator imageIterator = featuresForEachImage->begin();
	for (int i = 0; i < featuresForEachImage->size(); i++)
	{
		list<feature>::iterator featureIterator = imageIterator->begin();
		for (int j = 0; j < imageIterator->size(); j++)
		{
			list<pair<feature*, cv::Point>> keyword;
			keyword.push_front(pair<feature*, cv::Point>(&*featureIterator, cv::Point(0, 0)));

			list<list<feature>>::iterator imageIteratorFromBack = featuresForEachImage->end();
			--imageIteratorFromBack;//becasue for some reason it grabs the item past the real last
			for (int k = int(featuresForEachImage->size()) - 1; k > i; k--)
			{
				list<feature>::iterator featureIterator2 = imageIteratorFromBack->begin();
				for (int l = 0; l < imageIterator->size(); l++)
				{
					pair<feature*, cv::Point> thisFeature;
					thisFeature.first = &*featureIterator2;

					float threshold;
					GetConfigVarsFromID(CrossImageCorralationThresholdTrainer) >> threshold;

					if (DoesCorrelationReachThreshold(featureIterator->grayScale, featureIterator2->grayScale, 0.0, threshold, &thisFeature.second, true))
					{
						keyword.push_back(thisFeature);
					}

					++featureIterator2;
				}
				++featureIterator;
			}

			keywordLists.push_back(keyword);

			--imageIteratorFromBack;
		}
		++imageIterator;
	}
	
	return CreateKeywords(keywordLists, int(featuresForEachImage->size()) - ceil(featuresForEachImage->size()/5));
}

list<feature> CreateKeywords(list<list<pair<feature*, cv::Point>>> keywordLists, int thresholdOfSharedImages)
{
	list<feature*> featuresAlreadyInKeywords;

	list<feature> outPut;

	list<list<pair<feature*, cv::Point>>>::iterator keywordIterator = keywordLists.begin();
	for (int i = 0; i < keywordLists.size(); i++)
	{
		if (!(featuresAlreadyInKeywords.size() > 0 && find(featuresAlreadyInKeywords.begin(), featuresAlreadyInKeywords.end(), keywordIterator->begin()->first) != featuresAlreadyInKeywords.end()))//!(greater that 0 and if the first feature in the keywordIterator is in the list of featuresAlreadyInKeywords)
		{
			if (keywordIterator->size() >= thresholdOfSharedImages)
			{
				feature thisFeature = *keywordIterator->begin()->first;

				cv::Point mainImageOffSet(0, 0);

				list<pair<feature*, cv::Point>>::iterator keywordImageIterator = keywordIterator->begin();
				++keywordImageIterator;//the first one is the header image that all the other image will be added to
				for (int j = 1; j < keywordIterator->size(); j++)
				{
					AddImagesAt(
						&thisFeature.grayScale, 
						&(keywordImageIterator->first->grayScale), 
						&thisFeature.grayScale, 
						cv::Point(keywordImageIterator->second.x - mainImageOffSet.x, keywordImageIterator->second.y - mainImageOffSet.y), 
						&mainImageOffSet, 
						1 - (1 / (float(j) + 1)), 
						true);

					featuresAlreadyInKeywords.push_back(keywordImageIterator->first);
					
					for each (array<int, 4> range in keywordImageIterator->first->ranges)
						thisFeature.ranges.push_back(range);
					
					if(ShowImagesT)
						showImage(&(keywordImageIterator->first->grayScale));

					++keywordImageIterator;
				}
				if (ShowImagesT)
				{
					showImage(&keywordIterator->begin()->first->grayScale);
					showImage(&thisFeature.grayScale);
					cv::waitKey(0);
				}

				outPut.push_back(thisFeature);
			}
		}
		++keywordIterator;
	}

	return outPut;
}

int SaveMultipleAsImgFiles(std::string folderPath, std::string folderName, std::list<feature>* images)
{
	for each (feature image in *images)
	{
		SaveOneAsImgFile(folderPath, folderName, &image);
	}
	return 0;
}

int SaveOneAsImgFile(std::string folderPath, std::string folderName, feature* image)
{
	try
	{
		if (folderPath[0] == '\\' || folderPath[0] == '.')
			if(runPath == "\\" || runPath == "")
				folderPath = (TrimEnds(folderPath, "\\") + "\\");
			else
				folderPath = (runPath + "\\" + TrimEnds(folderPath, "\\") + "\\");

		boost::filesystem::path dir(folderPath);
		if (!boost::filesystem::exists(dir))
			if (!boost::filesystem::create_directory(dir))
				return -1;

		dir = (folderPath + folderName);
		if (!boost::filesystem::exists(dir))
			if (!boost::filesystem::create_directory(dir))
				return -1;
	}
	catch (const boost::filesystem::filesystem_error& ex)
	{
		cout << ex.what() << endl;
	}

	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	std::string filePath = folderPath + folderName + "\\" + to_string(uuid) + ".jpg";

	cout << filePath << endl;

	imwrite(filePath, image->grayScale);

	ofstream myfile;
	myfile.open(folderPath + folderName + "\\" + to_string(uuid) + ".vctrinf");
	myfile << "#Vector Info for keyword: " << uuid << endl;
	for each (std::array<int,4> vector in image->ranges)
	{
		myfile << "[" << endl;
		myfile << vector[0] << endl;
		myfile << vector[1] << endl;
		myfile << vector[2] << endl;
		myfile << vector[3] << endl;
		myfile << "]" << endl;
	}
	myfile.close();

	return 0;
}
