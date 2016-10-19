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

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <boost/locale.hpp>

extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
}

#include "./header/DetectRegions.hpp"
#include "./json/json.hh"

#if !defined VK_ESCAPE
	#define VK_ESCAPE 0x1B
#endif

using namespace std;
using namespace cv;
using namespace JSON;

static const int CAMERA_WIDTH = 640,
	CAMERA_HEIGHT = 480,
	BORDER = 0;

const char *windowName = "Business Card Recognition";
const char *resultFrame = "Recognized";
static mutex onGoing;
static bool printing = false;
static int shmid;
static key_t key;
static void *sharedMemoryRegion = NULL;
char *currentAddress = NULL;

void convertImageToArray(const Mat &image,
		Array &array);

static std::string fromLocale(const std::string &localeString){
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);
	std::locale locale = generator(boost::locale::util::get_system_locale());
	return boost::locale::conv::to_utf<char>(localeString, locale);
}

void initWebcam(VideoCapture &capture, int cameraNumber) {
	try {
		capture.open(cameraNumber);
	} catch(cv::Exception &e) {
		cerr << "Error : while opening camera" << endl;
	}

	if(!capture.isOpened()) {
		cerr << "Error : The camera is not opened" << endl;
		exit(1);
	}

	cout << "Loaded camera : " << cameraNumber << endl;
}

void showResult(vector<Mat> result) {

	Array resultArray;
	string payload;

	for(size_t i = 0;i < result.size();i++) {
		imshow(resultFrame, result[i]);
		usleep(250);
	}
	if(result.size()) {
		Object json;
		ostringstream stream;
		stream.str("");
		stream.clear();

		convertImageToArray(result[0], resultArray);

		json["image"] = resultArray;
		stream << json;
		payload = stream.str();

		const char *regionStart = payload.c_str();

		currentAddress = (char *)sharedMemoryRegion;
		currentAddress++;

		memcpy(currentAddress, regionStart, payload.length() + 1);

		currentAddress = (char *)sharedMemoryRegion;
		*currentAddress = '?';
		while(1) {
			if(*currentAddress == '!')
				break;
			sleep(1);
		}
	}

	onGoing.lock();
	printing = false;
	onGoing.unlock();
}

void process(VideoCapture &capture) {

	DetectRegions *region = new DetectRegions();

	cout << "Processing" << endl;
	while(true) {
		Mat currentFrame,
			displayFrame;

		vector<Mat> result;

		capture >> currentFrame;
		if(currentFrame.empty()) {
			cerr << "Error : while retrieving an image from the camera" << endl;
			continue;
		}

		currentFrame.copyTo(displayFrame);

		imshow(string(windowName), displayFrame);

		result = region->run(currentFrame);

		onGoing.lock();
		if(!printing) {
			printing = true;
			onGoing.unlock();
			result = region->run(currentFrame);

			thread detachable {showResult, result};
			detachable.detach();
		}
		else{
			onGoing.unlock();
		}

		char key = waitKey(10);
		if(key == VK_ESCAPE)
			break;

	}
}

void convertImageToArray(const Mat &image,
		Array &array) {
	Size size = image.size();

	for(size_t i = 0;i < size.width;i++) {
		Array eachArray;
		for(size_t j = 0;j < size.height;j++) {
			Point index(i, j);
			int elem = image.at<int>(index);
			eachArray.push_back(elem);
		}
		array.push_back(eachArray);
	}
}


int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage : ./BusinessCardRecognition <camera number> <shared memory key>" << endl;
	}

	int cameraNumber = atoi(argv[1]);
	key = (key_t)atoi(argv[2]);

	if((shmid = shmget(key,
					4194304,
					0600)) == -1) {
		perror("shmget");
		exit(1);
	}

	if((sharedMemoryRegion = shmat(shmid,
					(void *)0,
					0)) == (void *) -1) {
		perror("shmat");
		exit(1);
	}

	VideoCapture capture;
	Mat first;

	initWebcam(capture, cameraNumber);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

	capture >> first;
	printf("%d %d %d\n", first.total(), first.elemSize(), first.type());
	cout << "Done configuring camera" << endl;
	namedWindow(windowName);
	namedWindow(resultFrame);
	process(capture);
	return 0;
}

