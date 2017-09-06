# Keyword Object Detection
The idea is to make an image finding an algorithm that you by giving it training images. How it trains: it looks for common key images with detail that are prevalent in each image and makes a “keyword” out of that. Each key word also contains a vector that points to the center of the object. The keywords are put into a database so they easily can be accessed later. To find things in the image it used the “keywords” and uses a correlation algorithm to find the best match and then places its center where the vector places it. After all the keywords have been correlated, then the place with the most predicted center is the center of the object. This is explained more in depth in the hough section of the first one: [here](https://github.com/ZackJorquera/ComputerVisionLineAndCircleFinding).

# Theory
## Finding the object with the “keywords”
My first project (which is linked above at the top) uses edges to find circles and lines in an image. Instead of using edges this program will use common image samples (the keywords) to identify where the object is. Each keyword will be like a blurry key portion of the full image as to aid the correlation process. The keywords are stored in a database along with what it represents and a vector that points to the center of the object. Using this, the program finds the images equivalent (all that pass a correlation threshold) of what the keyword represents.  This is done for every keyword and with variations like rotation and scale. When each thing is correlated the vector is read and the resulting location (in an array or a list) from the image location plus the vector is given a vote for being a possible center. It is important to note that each item in the list is a really a range so that the program can account for errors in scaling and other discrete variation. Then that place with the most possibilities of being the object is identified with a box being drawn around the object.
## Making the keywords
This function will take an input of multiple similar images of the object in the same scenario. 
The program starts with the first image and splits it into boxes of features (uses a threshold image to find these boxes). It then repeats this step for all of the other images giving a lot of section of the images(add a case if there is a car with 2 wheels). Then it looks through each of the images to find if any of the features that are similar and adds those to a list like a stack of similar things. When finding the similarity the scale of the "Keyword" is varied as to aid in the find within the image. When the lists are made each image is combined to each other image at the point where the best correlation was relative to the first image. This will make a blurred image ready for use as a keyword. The range for the vector is given by the range at which each keyward lies within the image.

# Documentation
c++ Documentation can be found [Here](https://github.com/ZackJorquera/Keyword_Object_Detection/blob/master/Made%20In%20C/README.md) and Java documentation [Here](https://github.com/ZackJorquera/Keyword_Object_Detection/blob/master/Made%20In%20Java/README.md)

# Info 
Made By: Zack Jorquera

For: Mrs Root's IB/AP CS class at Centaurus High
