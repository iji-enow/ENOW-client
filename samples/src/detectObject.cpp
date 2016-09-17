/*
Copyright [2016] [JeaSung Park]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "../header/detectObject.hpp"

void detectObjectsCustom(const Mat &img,\
		CascadeClassifier &cascade,\
		vector<Rect> &objects,\
		int scaledWidth,\
		int flags,\
		Size minFeatureSize,\
		float searchScaleFactor,\
		int minNeighbors) {
	
	Mat gray,\
		inputImg,\
		equalizedImg;

	float scale;

	if(img.channels() == 3) {
		cvtColor(img, gray, CV_BGR2GRAY);
	}
	else if(img.channels() == 4){
		cvtColor(img, gray, CV_BGRA2GRAY);
	}
	else{
		gray = img;
	}

	scale = img.cols / (float)scaledWidth;
	if(img.cols > scaledWidth) {
		int scaledHeight = cvRound(img.rows / scale);
		resize(gray,\
				inputImg,\
				Size(scaledWidth, scaledHeight));
	}
	else{
		inputImg = gray;
	}

	equalizeHist(inputImg,\
			equalizedImg);

	cascade.detectMultiScale(equalizedImg,\
			objects,\
			searchScaleFactor,\
			minNeighbors,\
			flags,\
			minFeatureSize);

	if (img.cols > scaledWidth) {
		for (int i = 0; i < (int)objects.size(); i++ ) {
			objects[i].x = cvRound(objects[i].x * scale);
			objects[i].y = cvRound(objects[i].y * scale);
			objects[i].width = cvRound(objects[i].width * scale);
			objects[i].height = cvRound(objects[i].height * scale);
		}
	} 

	for (int i = 0; i < (int)objects.size(); i++ ) {
		if (objects[i].x < 0)
			objects[i].x = 0;
		if (objects[i].y < 0)
			objects[i].y = 0;
		if (objects[i].x + objects[i].width > img.cols)
			objects[i].x = img.cols - objects[i].width;
		if (objects[i].y + objects[i].height > img.rows)
			objects[i].y = img.rows - objects[i].height;
	}

}

void detectLargestObject(const Mat &source,\
		CascadeClassifier &cascade,\
		Rect &largestObject,\
		int scaledWidth) {

	int flags = CASCADE_FIND_BIGGEST_OBJECT;
	Size minFeatureSize = Size(20, 20);
	float searchScaleFactor = 1.1f;
	int minNeighbors = 4;
	vector<Rect> objects;

	detectObjectsCustom(source,\
			cascade,\
			objects,\
			scaledWidth,\
			flags,\
			minFeatureSize,\
			searchScaleFactor,\
			minNeighbors);

	if(objects.size() > 0){
		largestObject = (Rect)objects.at(0);
	}
	else{
		largestObject = Rect(-1, -1, -1, -1);
	}
}
