#include <iostream>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "Trainer.h"
#include "MongoReaderWriter.h"
#include "OpenCVTools.h"

using namespace std;


void GetInputImages(list<string>* imageFiles);
bool useRating(const feature& first, const feature& second);
void CorrelateImageFeatures(list<feature>* keyFeatures);


int Train(string arg)
{
	list<string> imageFiles;
	list<cv::Mat> *grayScaleimages;
	string objectName;

	cout << "Info: the center of the image will be used as the center of the object. for the best results please crop the object to the edges of the image so that only the object is classifyed as the object." << endl;
	
	GetInputImages(&imageFiles);

	cout << "please ender the name of the object in the images: ";
	cin >> objectName;

	grayScaleimages = LoadImages(imageFiles, cv::IMREAD_GRAYSCALE);

	/*
	if (arg == "-s")//shows the "keywords"
	{
		showImages(grayScaleimages);
		cv::waitKey(0);
	}

	ConnectToMongoDB();
	int test[4] = { 2,3,6,7 };
	WriteWordToMongo(objectName, test);
	//*/

	list<list<feature>> *keyFeaturesPerImage = new list<list<feature>>();

	//Gets all posable features that could be used for finding for each image
	list<cv::Mat>::iterator it = (*grayScaleimages).begin(); ////(*it) is the object in the list at i
	for (int i = 0; i < (*grayScaleimages).size(); i++)
	{
		list<feature> t = GetMostImportantPartsOfImage(&*it, 10, 10, 0,0,30,3,2);//TODO: add a config file that stores the file
		keyFeaturesPerImage->push_back(t);

		/*shows the images in t
		list<feature>::iterator it2 = t.begin(); // this is how you read through a list in c++
		for (int i = 0; i < t.size(); i++)
		{
			list<cv::Mat> temp;
			temp.push_back((*it2).featureGrayScale);
			showImages(&temp);
			++it2;
		}
		cv::waitKey(0);
		//*/
		++it;//goes to the next object in the list, does not throw if it is moved to one that does not exist to this will work fine
	}
	//*
	//TODO: add sub list image corralations to detect if two images or of the samething in each list
	list<list<feature>>::iterator keyFeaturesIterator = (*keyFeaturesPerImage).begin();//*it2
	for (int i = 0; i < keyFeaturesPerImage->size(); i++)
	{
		CorrelateImageFeatures(&*keyFeaturesIterator);
		++keyFeaturesIterator;
		//see if there are any duplicets in the list like two tires on a car and combine them this means there will be a list of vetor range things
	}
	//TODO: cross list corralation 
	//then add the images together (EVENYLY just halfing will not work)
	
	//*/

	return 0;
}

void CorrelateImageFeatures(list<feature>* keyFeatures)
{
	list<feature*> featuresToRemove;
	list<feature>::iterator featureIterator = (*keyFeatures).begin();
	for (int i = 0; i < (*keyFeatures).size(); i++)
	{
		list<feature>::iterator featureIterator2 = (*keyFeatures).end();
		--featureIterator2;//becasue for some reason it grabs the item past the real last
		for (int j = (*keyFeatures).size() - (i + 1); j > 0; j--)
		{
			float corralationThreshold = .85;//TODO: add config file
			cv::Point locationOfTemplate;
			feature* img1;
			feature* img2;

			float addImageRatio = 0.5;


			try
			{
				if ((*featureIterator).featureGrayScale.cols > (*featureIterator2).featureGrayScale.cols)//if(image 1 > image 2) <- in size
				{
					if (!DoesCorralationReachThreshold((*featureIterator).featureGrayScale, (*featureIterator2).featureGrayScale, 0.0, corralationThreshold, &locationOfTemplate))
					{
						--featureIterator2;
						continue;
					}
					img1 = &*featureIterator;//because  featureIterator is an iterator the star does not convert a point to an object it converts a list position to an object thus i have to then convert it to a pointer
					img2 = &*featureIterator2;
					//(*keyFeatures).erase(featureIterator2); <- ?? mabye add a list of ones to remove :TODO:
				}
				else
				{
					if (!DoesCorralationReachThreshold((*featureIterator2).featureGrayScale, (*featureIterator).featureGrayScale, 0.0, corralationThreshold, &locationOfTemplate))
					{
						--featureIterator2;
						continue;
					}
					if ((*featureIterator).featureGrayScale.cols == (*featureIterator2).featureGrayScale.cols && (*featureIterator).rating >= (*featureIterator2).rating)//if(image 1 == image 2 && image1.rating > image2.rating) <- in size
					{
						img1 = &*featureIterator;
						img2 = &*featureIterator2;
					}
					else
					{
						img1 = &*featureIterator2;
						img2 = &*featureIterator;
					}
				}

				AddImagesAt(&(img1->featureGrayScale), &(img2->featureGrayScale), &(img1->featureGrayScale), locationOfTemplate, 0.5f, true);
				for each(std::array<int, 4> range in img2->ranges)
				{
					range[0] = range[0] + locationOfTemplate.x;
					range[1] = range[1] + locationOfTemplate.y;
					range[2] = range[2] + locationOfTemplate.x;
					range[3] = range[3] + locationOfTemplate.y;
					img1->ranges.push_back(range);
				}
				showImage(&(img1->featureGrayScale));
				cv::waitKey(0);

				featuresToRemove.push_back(img2);


				--featureIterator2;
				continue;
			}
			catch (cv::Exception ex)
			{
				cout << ex.what() << endl;
			}
			catch (std::exception ex)
			{
				cout << ex.what() << endl;
			}
			catch (...)
			{

			}
			--featureIterator2;
		}
		++featureIterator;
	}
	
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
			(*imageFiles).push_back(thisImage);
		}
	}
}

list<feature> GetMostImportantPartsOfImage(cv::Mat *grayImage, int maxFeatures, float xStepSizePersentOfImage, float maxRot, float rotStep, float thresholdPresent, int maxfeatureSizeInSteps, int minfeatureSizeInSteps)
{
	using namespace cv;
	Mat gradImg;
	list<feature> Features;
	gradImg = GetGradientImage(*grayImage);

	int pps = int(gradImg.cols / (100 / xStepSizePersentOfImage));//pixels per step. i use this the get square steps

	for (int x = 0; x < int(gradImg.cols/pps + 0.5f); x++)// the + 0.5f founds so that if there is > 1/2 step left over it counts that as one more step
	{
		for (int y = 0; y < int(gradImg.rows / pps + 0.5f); y++)
		{
			for (int s = minfeatureSizeInSteps; s < maxfeatureSizeInSteps; s++)
			{
				feature f;
				f.featureGradientImage = MakeMatFromRange(cv::Point(x * pps, y * pps), cv::Point((x + s) * pps, (y + s) * pps), &gradImg);
				f.featureGrayScale = MakeMatFromRange(cv::Point(x * pps, y * pps), cv::Point((x + s) * pps, (y + s) * pps), grayImage);
				f.GetRating();
				if (f.rating < thresholdPresent)
					continue;
				std::array<int, 4> range;//for c++ you have to use std::array in order to add to the list
				range[0] = (gradImg.cols/2) - x;
				range[1] = ((gradImg.cols + 1) / 2) - x;
				range[2] = (gradImg.rows / 2) - y;
				range[3] = ((gradImg.rows + 1) / 2) - y;
				f.ranges.push_back(range);
				//TODO: fix the issue where the images are all the same thing but diffrent sizes
				Features.push_back(f);
			}
		}
	}

	if(Features.size() == 0)
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

