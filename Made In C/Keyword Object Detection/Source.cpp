#include <iostream>

#include "Trainer.h"
#include "Detector.h"

using namespace std;

int main(int argNums, char** argv)
{
	if (argNums < 2)
	{
		cout << "When calling the command use \"train\" to select training mode and \"find\" to select detection Mode." <<
			"For both options, you can add -s to show images along the way; this is not recommended for the training mode." <<
			"You can also use -ls to show just the last image." << endl;
		return -1;
	}

	if (string(argv[1]) == "train")
	{
		cout << "Training Mode:" << endl;
		if (argNums > 2)
			return Train(string(argv[2]));
		else
			return Train("");
	}
	else if (string(argv[1]) == "find")
	{
		cout << "Detection Mode:" << endl;
		if (argNums > 2)
			return Find(string(argv[2]));
		else
			return Find("");
	}
	else
	{
		cout << "Invalade selection:" << endl << "Selection input is case sensitive and all lower case" << endl;
		return -1;
	}
	return 0;
}
