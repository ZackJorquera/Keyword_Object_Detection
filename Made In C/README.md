
# Documentation

## Getting OpenCV to work with vs2017 (Visual Studios 2017)
This was a painful process in with hardly anything got done. In the end, everything works fine. The steps to get this to work are as follow:

### Step 1
Download the latest OpenCV windows package: [here](http://opencv.org/releases.html) and build to the c drive or the programs folder.

### Step 2
In vs2017 create a new project called "OpenCV x64 Template" and when it loads create a new source called "source.cpp" under the "Source Files" folder. in the properties panel under C/C++ -> General add the include folder from opencv\build to the "Additional Include Directories: box.
Under Linker -> General and add the lib (library) folder to the "Additional Library Directories" box from opencv\build\x64\vc14\lib.
Under Linker -> Input add the .lib file to "Additional Dependencies" box adding the release library and the debug library respectively e.x. opencv_worldxxxd.lib for debug and opencv_worldxxx.lib for release.

### Step 3
Still in the properties panel under Build Events -> Pre-Build Event in the "Command Line" box put something to copies the openCV dll to the output folder like ```copy "$(ProjectDir)..\OpenCVBin\opencv_worldxxx.dll" "$(TargetDir)"``` where there is an extra d for debug.

### Step 4
For future use, you can export it as a visual studios project template through Project -> Export Template.

## MongoDB c++ driver
Download and instruction are [here](http://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/). I use boost with mongo.

[MongoDB](https://www.mongodb.com/) is an open source, free, NoSQL database service that has drivers for c#, c++, java, and more. The reason I chose to use mongo is that I will be needing to store the keywords some how and mongo has the functionality to let me do this easily (some what). mongo allows me to add and find items in the form of a JSON document(well, really it is BSON) with in millisecond even with millions of document. with mongo, you can have small images stored up to a 16mb limit per document.
