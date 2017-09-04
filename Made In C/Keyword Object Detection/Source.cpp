#include <iostream>

#include "Trainer.h"
#include "Detector.h"

using namespace std;

int main(int argNums, char** argv)
{
	if (argNums < 2)
	{
		cout << "Use \"train\" to select training mode and \"find\" to select detection Mode" << endl;
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
		return Find();
	}
	else
	{
		cout << "Invalade selection:" << endl << "Selection input is case sensitive and all lower case" << endl;
		return -1;
	}
	return 0;
}
