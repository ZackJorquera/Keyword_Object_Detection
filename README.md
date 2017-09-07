# Keyword Object Detection

The idea is to make an image finding algorithm that can be trained by giving it similar images of the same thing. How it trains: it looks for common key images with detail that are prevalent in each image and makes a “keyword” out of that. Each key word also contains a vector that points to the center of the object. The "keywords" are put into a database so they easily can be accessed later. To find things in the image it used the “keywords” and uses a correlation algorithm to find the best match and then places its center where the vector places it. After all the "keywords" have been correlated, then the place with the most predicted center is the center of the object. This concept is explained more in depth in the hough section of my ComputerVisionLineAndCircleFinding project, which can be found [here](https://github.com/ZackJorquera/ComputerVisionLineAndCircleFinding).

# Theory

## Finding the object with the “keywords”

My first project (which is linked above at the top) uses edges to find circles and lines in an image. Instead of using edges this program will use common image samples (the "keywords") to identify where and what the object is. Each "keyword" will be like a blurry key portion of the full image as to aid the correlation process. The "keywords" are stored in a database along with what it represents and a vector that points to the center of the object. Using this technique, the program finds the images equivalent (all that pass a correlation threshold) of what the "keyword" represents. This is done for every "keyword" and with variations like rotation and scale. When each "keyword" is correlated the vector is read and the possible object location is given from the image's location plus the vector plus the vector. It is important to note that each vector in the list is a really a range so that the program can account for errors in scaling and other discrete variation. Then that place with the most possibilities of being the object is identified with a box being drawn around the object.

## Making the "keywords"

This function will take an input of multiple similar images of the object in the same scenario. 
The program starts with the first image and splits it into boxes of features (using a gradient image to find these boxes). It then repeats this step for all of the other images giving a lot of features of the images. Then it looks through each of the images to find if any of the features are similar and then adds those to a list in a stack (although I'm just going to use a list). When finding the similarity the scale of the "Keyword" is varied as to compensate for different image sizes. When the lists are made each image is combined with each other image at the point where the best correlation is relative to the first image. This will make a "blurred" image ready for use as a "keyword". The range for the vector is given by the range at which each "keyword" lies within the image.

# Documentation

c++ Documentation can be found [Here](https://github.com/ZackJorquera/Keyword_Object_Detection/blob/master/Made%20In%20C/README.md)
and Java documentation [Here](https://github.com/ZackJorquera/Keyword_Object_Detection/blob/master/Made%20In%20Java/README.md)

# Info 

Made By: Zack Jorquera

For: Mrs. Root's IB/AP CS class at Centaurus High
