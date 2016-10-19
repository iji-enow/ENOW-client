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

#include "../header/Region.hpp"

extern const char *resultFrame;

Region& Region::operator=(const Region &T) {
	if(!T.img_transformmed.empty()) {
		T.img_transformmed.copyTo(img_transformmed);
	}

	if(!T.corners.empty()) {
		corners = T.corners;
	}

	biased = T.biased;
	return *this;
}

Region& Region::operator=(Region &T) {
	if(!T.img_transformmed.empty()) {
		T.img_transformmed.copyTo(img_transformmed);
	}

	if(!T.corners.empty()) {
		corners = T.corners;
	}

	biased = T.biased;
	return *this;
}

Region Region::operator-(const Region &T) const{
	if(corners.size() != 4 || T.corners.size() != 4) {
		Region *empty = new Region();
		return *empty;
	}

	vector<Point> diff;

	for(size_t i = 0;i < 4;i++) {
		Point minus = corners[i] - T.corners[i];
		diff.push_back(minus);
	}

	Region *result = new Region(diff);

	return *result;
}

float Region::square() {
	if(corners.empty()) {
		return -1;
	}

	float accumulated = 0;

	for(size_t i = 0;i < 4;i++) {
		accumulated += corners[i].dot(corners[i]);
	}

	return accumulated;
}

float Region::perimeter() {
	float sum = 0,
		  dx, dy;

	for(size_t i = 0;i < corners.size();i++) {
		size_t index = (i + 1) % corners.size();

		dx = corners[i].x - corners[index].x;
		dy = corners[i].y - corners[index].y;

		sum += sqrt(pow(dx, 2) + pow(dy, 2));
	}

	return sum;
}

void Region::print(const Mat &input) {
	Mat img_copy;
	input.copyTo(img_copy);
	for(size_t i = 0;i < corners.size();i++) {
		size_t index = (i + 1) % corners.size();
		cv::line(img_copy,
				corners[i], corners[index],
				CV_RGB(255, 255, 0),
				3,
				CV_AA);
	}

	imshow(resultFrame, img_copy);
}

void Region::warpMatrix(const Mat &input, Mat &output, Size &size) {
	vector<Point2f> regionCorners2D;
	vector<Point2f> f_corners;

	for(size_t i = 0;i < corners.size();i++) {
		f_corners.push_back(corners[i]);
	}

	if(biased) {
		regionCorners2D.push_back(Point2f(size.width - 1, 0));
		regionCorners2D.push_back(Point2f(0, 0));
		regionCorners2D.push_back(Point2f(0, size.height - 1));
		regionCorners2D.push_back(Point2f(size.width - 1, size.height - 1));

	} else {
		regionCorners2D.push_back(Point2f(0, 0));
		regionCorners2D.push_back(Point2f(0, size.height - 1));
		regionCorners2D.push_back(Point2f(size.width - 1, size.height - 1));
		regionCorners2D.push_back(Point2f(size.width - 1, 0));
	}

	Mat transformMatrix = getPerspectiveTransform(f_corners, regionCorners2D);
	warpPerspective(input, output,
			transformMatrix,
			size);

	imshow(resultFrame, output);
}

bool Region::verifySize() {

	float error = 0.07,
		  aspect_median = 1.696296,
		  minimumAspect = aspect_median - aspect_median * error,
		  maximumAspect = aspect_median + aspect_median * error;

	int minimumArea = (int)(pow(120, 2) * aspect_median),
		maximumArea = (int)(pow(360, 2) * aspect_median);

	if(corners.size() != 4)
		return false;

	Point height_vector = corners[0] - corners[1],
		  width_vector = corners[1] - corners[2];
	float width = sqrt(width_vector.dot(width_vector)),
		  height = sqrt(height_vector.dot(height_vector));
	float area = width * height,
		  aspect = width / height;

	if(aspect < 1) {
		aspect = height / width;
		biased = true;
	}

	if( (area < minimumArea || area > maximumArea) ||
			(aspect < minimumAspect || aspect > maximumAspect) ) {
		return false;
	}

	return true;
}
