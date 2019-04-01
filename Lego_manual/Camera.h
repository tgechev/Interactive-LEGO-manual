#ifndef CAMERA_H
#define CAMERA_H
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <uEye.h>
#include<mutex>

class Camera {
private:
	HIDS hCam;
	cv::VideoCapture camStream;
	std::mutex myMutex;

	//image mem vars
	char* pMem = NULL;
	int memID = 0;

	const int width = 1280;
	const int height = 1024;
public:
	Camera(UINT _devID);
	void Initialize();
	cv::Mat getFrame();
	//cv::VideoCapture getStream();
	void Close();
};
#endif