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

#ifndef _PREPROCESS_FACE_
#define _PREPROCESS_FACE_

#include <stdio.h>
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool getPreprocessedFace(Mat &source,\
		int faceWidth,\
		CascadeClassifier &faceCascade,\
		Rect *faceRegion = NULL);

#endif
