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

#ifndef _DETECT_REGIONS_HPP_
#define _DETECT_REGIONS_HPP_

extern "C"{
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
}

#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include <opencv2/opencv.hpp>

#include "./Region.hpp"

using namespace std;
using namespace cv;

class DetectRegions {

	private:
		Size RegionSize;

		vector<Mat> segment(Mat &input);
		void getGray(const Mat &image,
				Mat &img_gray);
		void performThreshold(const Mat &img_gray,
				Mat &img_thresholdImg);
		void findContours(const Mat &img_threshold,
				vector<vector<Point> > &contours,
				int minContourPointsAllowed);
		void findRegionCandidates(vector<vector<Point> > &contours,
				vector<Region> &detectedRegion,
				const Mat &input);
		void warpMatrix(vector<Region> &regions,
				const Mat &input,
				vector<Mat> &img_warped);
		Mat histeq(Mat &in);
	public:
		DetectRegions() : RegionSize(Size(180, 100)) {
			saveRegions = false;
			showSteps = false;
		}
		bool saveRegions;
		bool showSteps;
		vector<Mat> run(Mat &input);
};

#endif
