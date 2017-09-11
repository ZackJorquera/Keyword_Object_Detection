# Documentation

## Getting OpenCV to work with vs2017 (Visual Studios 2017)
This was a long process and some issues were not well-documented due to the fact that the documentation was made for the 2010 version. In the end, everything works fine. The original documentation can be found [here](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_visual_studio_Opencv/windows_visual_studio_Opencv.html). The steps to get this to work are as follow:

### Step 1
Download the latest OpenCV windows package: [here](http://opencv.org/releases.html) and build to the c drive or the programs folder.

### Step 2
In vs2017 create a new project called "OpenCV x64 Template" and when it loads, create a new source called "source.cpp" under the "Source Files" folder. in the properties panel under C/C++ -> General add the include folder from opencv\build to the "Additional Include Directories: box.
Under Linker -> General and add the lib (library) folder to the "Additional Library Directories" box from opencv\build\x64\vc14\lib.
Under Linker -> Input add the .lib file to "Additional Dependencies" box adding the release library and the debug library respectively e.x. opencv_worldxxxd.lib for debug and opencv_worldxxx.lib for release.

### Step 3
Still in the properties panel under Build Events -> Pre-Build Event in the "Command Line" box put something to copies the openCV dll to the output folder like ```copy "$(ProjectDir)..\OpenCVBin\opencv_worldxxx.dll" "$(TargetDir)"``` where there is an extra d for debug.

### Step 4
For future use, you can export it as a visual studios project template through Project -> Export Template.

## MongoDB c++ driver
Download and instruction are [here](http://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/). I use boost with mongo.

[MongoDB](https://www.mongodb.com/) is an open source, free, NoSQL database service that has drivers for c#, c++, java, and more. The reason I chose to use mongo is that I will be needing to store the keywords some how and mongo has the functionality to let me do this easily (some what). mongo allows me to add and find items in the form of a JSON document(well, really it is BSON) with in millisecond even with millions of document. with mongo, you can have small images stored up to a 16mb limit per document.

## Getting OpenCV to work
a lot of unforeseen things happened when doing this, that is why this is two days past the deadline and still not completed. the unforeseen things that happened include:

- When the features are found there are duplicates of the same image but in different sizes
- no correlation algorithm exists in OpenCV that searches out of frame
- a gradient image of a gray scale image has a different size
- more things that I forgot about that are less affecting to the code

What I did in the update of [74e81da](https://github.com/ZackJorquera/Keyword_Object_Detection/commit/74e81dabaa1c1d03880c0b3baafe2c00c9933d5c#diff-e1951ecf1e6d4e22fd96296ad8b3fb43):

- I made a function that gets that most relevant feature in the image by using a gradient image that shows edges and how strong those edges are from the brightness.
- Made a function that finds the correlation between two images and return that offset value (still being made)
- a function that adds two images together with an offset and returns an image that is a square around the intersection of the image
- some other minor image transformation stuff that openCV did not have the way I wanted it

in total, I mostly finished the feature correlation for the same image and made code to be used for the cross image correlation.

## making the CorrelateFeaturesCrossImage function
The goal was to have every image in each list look at all the other images lists and find what matches. at each iteration of this, a new list of features is made of the matching images where the first object in the list is the primary images and following that are all of the matches with the location at which they matched. When all cycles finish and a list of lists of features is made acting as a list of keywords waiting to be made it then cycles through that adding all the image together at every level. in this prosses, it skips every list which head feature is a feature which was in a previous list.

## Remaking DoesCorrelationReachThreshold
This change ([]()) was a ness ashery thing to make because the OpenCV template matcher only maTched when the templated fully fit in the image. The change of this function allowed for two same size images and be correlated to get the best spot. This algorithm uses the non-normalized differences of the two image the gradient images should be used to get the best results.
