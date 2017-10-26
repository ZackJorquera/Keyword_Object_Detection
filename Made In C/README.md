# Build log

## Getting OpenCV to work with vs2017 (Visual Studios 2017)
This was a long process, and some issues were not well-documented due to the fact that the documentation was made for the 2010 version. In the end, everything works fine. The original documentation can be found [here](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_visual_studio_Opencv/windows_visual_studio_Opencv.html). The steps to get this to work are as follow:

### Step 1
Download the latest OpenCV windows package: [here](http://opencv.org/releases.html) and build to the c drive or the programs folder.

### Step 2
In vs2017 create a new project called "OpenCV x64 Template" and when it loads, create a new source called "source.cpp" under the "Source Files" folder. In the properties panel under C/C++ -> General add the include folder from opencv\build to the "Additional Include Directories: box.
Under Linker -> General and add the lib (library) folder to the "Additional Library Directories" box from opencv\build\x64\vc14\lib.
Under Linker -> Input add the .lib file to "Additional Dependencies" box adding the release library and the debug library respectively e.x. opencv_worldxxxd.lib for debug and opencv_worldxxx.lib for release.

### Step 3
Still in the properties panel under Build Events -> Pre-Build Event in the "Command Line" box put something to copies the OpenCV dll to the output folder like ```copy "$(ProjectDir)..\OpenCVBin\opencv_worldxxx.dll" "$(TargetDir)"``` where there is an extra d for debug.

### Step 4
For future use, you can export it as a visual studios project template through Project -> Export Template.

## MongoDB c++ driver
Download and instruction are [here](http://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/). I use boost with mongo.

[MongoDB](https://www.mongodb.com/) is an open source, free, NoSQL database service that has drivers for c#, c++, java, and more. The reason I chose to use mongo is that I will need to store the keywords somehow and mongo has the functionality to let me do this easily (somewhat). Mongo allows me to add and find items in the form of a JSON document(well, really it is BSON) within millisecond even with millions of document. With mongo, you can have small images stored up to a 16mb limit per document.

## Getting OpenCV to work
A lot of unforeseen things happened when doing this, that is why this is two days past the deadline and still not completed. The unforeseen things that happened include:

- When the features are found there are duplicates of the same image but in different sizes
- no correlation algorithm exists in OpenCV that searches out of frame
- a gradient image of a grayscale image has a different size
- more things that I forgot about that are less affecting to the code

What I did in the update of [74e81da](https://github.com/ZackJorquera/Keyword_Object_Detection/commit/74e81dabaa1c1d03880c0b3baafe2c00c9933d5c#diff-e1951ecf1e6d4e22fd96296ad8b3fb43):

- I made a function that gets that most relevant feature in the image by using a gradient image that shows edges and how strong those edges are from the brightness.
- Made a function that finds the correlation between two images and return that offset value (still being made)
- a function that adds two images together with an offset and returns an image that is a square around the intersection of the image
- some other minor image transformation stuff that openCV did not have the way I wanted it

in total, I mostly finished the feature correlation for the same image and made code to be used for the cross image correlation.

## making the CorrelateFeaturesCrossImage function
The goal was to have every image in each list look at all the other images lists and find what matches. At each iteration of this, a new list of features is made of the matching images where the first object in the list is the primary images and following that are all of the matches with the location at which they matched. When all cycles finish, and a list of lists of features is made acting as a list of keywords waiting to be made it then cycles through that adding all the image together at every level. In this prosses, it skips every list which head feature is a feature which was in a previous list.

## Remaking DoesCorrelationReachThreshold
This change was a necessary thing to make because the OpenCV template matcher only maTched when the templated fully fit in the image. The change of this function allowed for two same size images and be correlated to get the best spot. This algorithm uses the non-normalized differences of the two image the gradient images should be used to get the best results.
 
 At [e0f816a](https://github.com/ZackJorquera/Keyword_Object_Detection/commit/e0f816a51fc5a1ca6422729a39772c7d8e69706f) I fixed how CorrelateImageFeatures works by removing the featuresToRemove list and having a system that adds all the images at the end. This change allows for every image to be evenly combined and no extras to be introduced.

## Removing mongo
Im no longer going to use mongo, I have moved to just saving the images in a jpg and having another file to save the vector data. Each file is named a random GUID as not to need to deal with two files of the same name. Because of this change, I have fully removed the mongo CP files. 

I have also started to use boost to write to the files.

## Detection
This function just reads the data from the saved feature folders and then finds the thing with the keywords. For reading the folder system, I used boost because its simplicity in doing things like this.  The function that accumulates the possible locations for the objects is roughly what was in the line and circle finding algorithm which means this it was fairly simple to make the accumulator. To make it even easier I used OpenCV's pre-build matchTemplate function.

## Detection 2
for now i just have it drawing a dot and with testing it can succesfuly find cars.



# Documentation
This code has two parts:
1. The Trainer
     1. Take input of n training images 
     2. For each training image, the program finds the most complicated parts
     3. It looks for similarities in its image and then every other image
     4. Comins all the similar images to make keywords
     5. Writes the keywords to a file
2. The Detector
     1. Reads the keywords that were made in the Trainer
     2. correlates each keyword
     3. find the best spots for different keywords
     4. draws boxes around the keywords
