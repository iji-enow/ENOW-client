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

/*
========================================
	WebcamFaceDetectionSample
========================================
	Description :
		 WebcamFaceDetectionSample is a sample program that demonstrate how to
		 use MQTTClient program. The program first detects any human face and
		 sends it to the broker connected with MQTTClient
	Command line arguments :
	 	* 1st arguments : Default camera number
		* 2nd arguments : key value mapped with allocated IPC resource
*/

extern "C"{
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
}
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "./header/detectObject.hpp"
#include "./header/preprocessFace.hpp"
#include "./header/ImageUtils.h"
#include "../json/json.hh"

using namespace std;
using namespace cv;
using namespace JSON;

#if !defined VK_ESCAPE
	#define VK_ESCAPE 0x1B
#endif

const char *faceCascadeFilename = "/home/ubuntu/Repository/api/samples/xml/lbpcascade_frontalface.xml";

const int faceWidth = 70;
const int faceHeight = 70;
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;

static int key = 0;
static bool onTransfer = false;
static int count = 0;
static mutex transfer_mutex;

static int sharedMemoryID = 0;
static key_t sharedKey;
static void *sharedMemoryRegion = NULL;

const char *windowName = "Webcam Face Detection Sample";
const int BORDER = 8;

template <typename T> string toString(T t){
	ostringstream out;
	out << t;
	return out.str();
}

template <typename T> T fromString(string t){
	T out;
	istringstream in(t);
	in >> out;
	return out;
}

void initClassifier(CascadeClassifier &faceCascade){

	try {
		faceCascade.load(faceCascadeFilename);
	} catch(cv::Exception &e) {
		perror("load");
	}

	if(faceCascade.empty()) {
		fprintf(stderr, "Error: Could not load Face Detection cascade classifier\n");
		exit(1);
	}

	printf("Loaded the Face Detection cascade classifier\n");

}

void initCamera(VideoCapture &capture,\
		int cameraNumber){
	try {
		capture.open(cameraNumber);
	} catch(cv::Exception &e){

	}

	if(!capture.isOpened()) {
		fprintf(stderr, "Error : Could not access the camera\n");
		exit(1);
	}

	printf("Loaded camera\n");
}

static void shareMemory(void) {
	transfer_mutex.lock();
	if(onTransfer){
		transfer_mutex.unlock();
	}else{
		onTransfer = true;
		transfer_mutex.unlock();

		cout << "GOT FACE; ON TRANSFER->" << onTransfer << endl;

		char *currentAddress = (char *)sharedMemoryRegion;
		currentAddress++;

		Object json;
		ostringstream stream;
		string payload;
		json["present"] = string("person");
		stream << json;
		payload = stream.str();
		cout << "Sending payload to host program" << endl;
		cout << "	" << payload << endl;
		memcpy(currentAddress, payload.c_str(), strlen(payload.c_str()) + 1);

		currentAddress = (char *)sharedMemoryRegion;
		*currentAddress = '?';

		while(1){
			if(*currentAddress == '!')
				break;
			sleep(1);
		}

		transfer_mutex.lock();
		onTransfer = false;
		transfer_mutex.unlock();
	}
}

void detect(VideoCapture &capture,\
		CascadeClassifier &faceCascade) {

	while(true) {
		Mat cameraFrame,\
			displayedFrame,\
			preprocessedFrame;
		Rect faceRegion;

		bool gotFace = false;
		char key;

		capture >> cameraFrame;
		if(cameraFrame.empty()) {
			fprintf(stderr, "Error : Could not capture the current frame\n");
			exit(1);
		}

		cameraFrame.copyTo(displayedFrame);

		gotFace = getPreprocessedFace(displayedFrame,\
				faceWidth,\
				faceCascade,\
				&faceRegion);

		if(gotFace){
			thread transfer {shareMemory};
			transfer.detach();
		}

		if (faceRegion.width > 0) {
			rectangle(displayedFrame, faceRegion, CV_RGB(255, 255, 0), 2, CV_AA);
		}

		imshow(windowName, displayedFrame);

		key = waitKey(10);
		if(key == VK_ESCAPE)
			break;
	}
}

int main(int argc, char *argv[]) {

	CascadeClassifier faceCascade;
	VideoCapture capture;

	int cameraNumber = 0;

	initClassifier(faceCascade);

	if(argc > 1)
		cameraNumber = atoi(argv[1]);

	key = atoi(argv[2]);
	sharedKey = (key_t)key;

	if((sharedMemoryID = shmget(sharedKey,\
					BUFSIZ,\
					0600)) < 0){
		perror("shmget");
		exit(1);
	}

	if((sharedMemoryRegion = shmat(sharedMemoryID,\
					(void *)0,\
					0)) == (void *)-1){
		perror("shmat");
		exit(1);
	}

	initCamera(capture, cameraNumber);

	capture.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

	namedWindow(windowName);

	detect(capture, faceCascade);

	return 0;
}
