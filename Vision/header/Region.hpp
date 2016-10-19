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

#ifndef _REGION_
#define _REGION_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

extern "C"{
#include <math.h>
}

using namespace std;
using namespace cv;

class Region {

	private:
		Mat img_transformmed;
		vector<Point> corners;
		bool biased;

	public:
		Region() : biased(false){ 
		}
		Region(vector<Point> &_corners) : corners(_corners), biased(false) {

			Point vector1 = corners[1] - corners[0];
			Point vector2 = corners[2] - corners[0];

			double convexHull = (vector1.x * vector2.y) - (vector1.y * vector2.x);

			if(convexHull < 0.0) {
				swap(corners[1], corners[3]);
			}

		}
		Region(Mat &_img_transformmed,
				vector<Point> &_corners) : img_transformmed(_img_transformmed), corners(_corners), biased(false) {

		}
		Region(const Region &T) : biased(false) {
			if(!T.img_transformmed.empty()) {
				T.img_transformmed.copyTo(img_transformmed);
			}

			if(!T.corners.empty()) {
				corners = T.corners;
			}

			biased = T.biased;
		}
		Region& operator=(const Region &T);
		Region& operator=(Region &T);
		Region operator-(const Region &T) const;
		void warpMatrix(const Mat &input, Mat &output, Size &size);
		bool verifySize();
		void print(const Mat &input);
		float square();
		float perimeter();
};

#endif
