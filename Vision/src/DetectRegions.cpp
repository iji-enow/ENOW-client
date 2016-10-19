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

#include "../header/DetectRegions.hpp"

extern const char *windowName;
extern const char *resultFrame;

void DetectRegions::getGray(const Mat &image,
		Mat &img_gray) {
	if(image.channels() == 3)
		cvtColor(image, img_gray,
				CV_BGR2GRAY);
	else if(image.channels() == 4)
		cvtColor(image, img_gray,
				CV_BGRA2GRAY);
	else if(image.channels() == 1)
		img_gray = image;
}

void DetectRegions::performThreshold(const Mat &img_gray,
		Mat &img_threshold) {
	adaptiveThreshold(img_gray,
			img_threshold,
			255,
			ADAPTIVE_THRESH_MEAN_C,
			THRESH_BINARY_INV,
			7,
			7);
}

void DetectRegions::findContours(const Mat &img_threshold,
		vector<vector<Point> > &contours,
		int minContourPointsAllowed) {

	Mat img_threshold_copy;
	
	img_threshold.copyTo(img_threshold_copy);

	vector<vector<Point> > allContours;
	cv::findContours(img_threshold_copy,
			allContours,
			CV_RETR_LIST,
			CV_CHAIN_APPROX_NONE);

	contours.clear();
	for(size_t i = 0;i < allContours.size();i++) {
		int contourSize = allContours[i].size();
		if(contourSize > minContourPointsAllowed) {
			contours.push_back(allContours[i]);
		}
	}
}

void DetectRegions::findRegionCandidates(vector<vector<Point> > &contours,
		vector<Region> &detectedRegion,
		const Mat &input) {
	vector<Point> approxCurve;
	vector<Region> possibleRegion;
	vector<pair<int, int> > tooNearCandidates;
	Mat img_copy;

	input.copyTo(img_copy);
	
	for(size_t i = 0;i < contours.size();i++) {
		
		approxCurve.clear();

		approxPolyDP(contours[i],
			   	approxCurve,
			   	double(contours[i].size()) * 0.05,
			   	true);

		if(approxCurve.size() != 4)
			continue;

		if(!isContourConvex(approxCurve))
			continue;

		float minDist = 640000.0f;
		for(size_t i = 0;i < 4;i++) {
			Point vec = approxCurve[i] - approxCurve[(i + 1) % 4];
			float squareDistance = vec.dot(vec);
			minDist = min(minDist, squareDistance);
		}

		if(minDist > 14400.0f){
			Region region(approxCurve);
			possibleRegion.push_back(region);
		}
	}

	for(size_t i = 0;i < possibleRegion.size();i++) {
		const Region &region1 = possibleRegion[i];
		for(size_t j = i + 1;j < possibleRegion.size();j++) {
			const Region &region2 = possibleRegion[j];
			Region diff = region1 - region2;
			float distSquared = diff.square();

			if(distSquared != -1) {
				distSquared /= 4;
				if(distSquared < 900) 
					tooNearCandidates.push_back(pair<int, int>(i, j));
			}	
		}
	}

	vector<bool> removalMask(possibleRegion.size(), false);
	for(size_t i = 0;i < tooNearCandidates.size();i++) {
		float p1 = possibleRegion[tooNearCandidates[i].first].perimeter();
		float p2 = possibleRegion[tooNearCandidates[i].second].perimeter();

		size_t removalIndex = (p1 > p2) ?\
							  tooNearCandidates[i].second :\
							  tooNearCandidates[i].first;
		removalMask[removalIndex] = true;
	}

	detectedRegion.clear();
	float maxPerimeter = 0.0f;
	size_t index = -1;
	for(size_t i = 0;i < possibleRegion.size();i++) {
		if(!removalMask[i]) {
			if(possibleRegion[i].verifySize()){
				if(maxPerimeter < possibleRegion[i].perimeter())
					index = i;
			}
		}
	}
	if(index != -1)
		detectedRegion.push_back(possibleRegion[index]);
}

void DetectRegions::warpMatrix(vector<Region> &regions,
		const Mat &input,
		vector<Mat> &img_warped) {
	for(size_t i = 0;i < regions.size();i++) {
		Mat result;
		regions[i].warpMatrix(input, result, RegionSize);
		img_warped.push_back(result);
	}
}

vector<Mat> DetectRegions::run(Mat &input) {
	vector<Mat> tmp = segment(input);
	return tmp;
}

vector<Mat> DetectRegions::segment(Mat &input) {

	Mat img_gray,\
		img_threshold;
	vector<Region> output;
	vector<vector<Point> > contours;
	vector<Mat> result;

	getGray(input,
			img_gray);
	performThreshold(img_gray,
			img_threshold);
	findContours(img_threshold,
			contours,
			img_gray.cols / 5);
	findRegionCandidates(contours,
			output,
			input);
	if(!output.empty()) {
		warpMatrix(output,
				input,
				result);
	}

	return result;
}
