#include "Camera.h"

using namespace std;
using namespace cv;

const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

static void on_low_H_thresh_trackbar(int, void *)
{
	low_H = min(high_H - 1, low_H);
	setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
	high_H = max(high_H, low_H + 1);
	setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
	low_S = min(high_S - 1, low_S);
	setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
	high_S = max(high_S, low_S + 1);
	setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
	low_V = min(high_V - 1, low_V);
	setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", window_detection_name, high_V);
}

int main() {

	Camera* cam = new Camera(1);

	cam->Initialize();

	cam->SetROI(0, 0, 1152, 964);

	Mat camFrame, frame_HSV, frame_threshold;

	Mat canny_output,gray;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	namedWindow(window_capture_name, WINDOW_NORMAL);
	namedWindow(window_detection_name, WINDOW_NORMAL);

	// Trackbars to set thresholds for HSV values
	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);

	/*createButton(whiteBtn, radioSelect, (void*)&whiteBtn, QT_RADIOBOX);
	createButton(blackBtn, radioSelect, (void*)&blackBtn, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);
	createButton("button5", callbackButton1, NULL, QT_RADIOBOX);*/

	while (true) {

		camFrame = cam->getFrame();

		flip(camFrame, camFrame, -1);

		// Convert from BGR to HSV colorspace
		cvtColor(camFrame, frame_HSV, COLOR_BGR2HSV);
		// Detect the object based on HSV Range Values
		inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_threshold);
		// Show the frames
		imshow(window_capture_name, camFrame);
		imshow(window_detection_name, frame_threshold);


		//Canny(gray, canny_output, 50, 150, 3);
		//findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		


		if (waitKey(30) >= 0)
			break;
	}

	cam->Close();


	return 0;
}