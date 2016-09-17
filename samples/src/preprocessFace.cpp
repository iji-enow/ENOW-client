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

#include "../header/preprocessFace.hpp"
#include "../header/ImageUtils.h"
#include "../header/detectObject.hpp"

bool getPreprocessedFace(Mat &source,\
		int faceWidth,\
		CascadeClassifier &faceCascade,\
		Rect *faceRegion) {

	int faceHeight = faceWidth;
	Rect faceRect;
	
	if(faceRegion)
		faceRegion->width = -1;

	detectLargestObject(source,\
			faceCascade,\
			faceRect);

	if(faceRect.width > 0) {
		if(faceRegion)
			*faceRegion = faceRect;
		return true;
	}

	return false;
}



