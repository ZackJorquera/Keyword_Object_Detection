# Documentation

## Getting OpenCV to work with vs2017 (Visual Studios 2017)
This was a painful process in with hardly anything got done. In the end, everything works fine. The steps to get this to work are as fallow:

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
