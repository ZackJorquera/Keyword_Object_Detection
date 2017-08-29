#include <iostream>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "Trainer.h"
#include "MongoReaderWriter.h"
#include "OpenCVTools.h"

using namespace std;


void showImages(list<cv::Mat>);


int Train(string arg)
{
	list<string> imageFiles;
	string objectName;

	cout << "Info: the center of the image will be used as the center of the object. for the best results please crop the object to the edges of the image so that only the object is classifyed as the object." << endl;
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
			imageFiles.push_front(thisImage);
		}
	}
	cout << "please ender the name of the object in the images: ";
	cin >> objectName;

	if (arg == "-s")
	{
		showImages(*LoadImages(imageFiles));
		cv::waitKey(0);
	}

	ConnectToMongoDB();
	int test[4] = { 2,3,6,7 };
	WriteWordToMongo(objectName, test);

	return 0;
}


