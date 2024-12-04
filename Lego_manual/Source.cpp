#include "Camera.h"
#include<time.h>
using namespace std;
using namespace cv;

const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
const String difference = "difference";
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;


int thresh = 255;
int max_thresh = 255;
RNG rng(12345);
Mat image, image_gray;
int largest_area = 0;
int largest_contour_index = 0;
Point area0;
Rect bounding_rect;


int low_thresh = 89;
int max_thresh1 = 255;

Mat camFrame, frame_HSV, frame_threshold;
bool urMom = true;
bool urMom1 = true;
Mat newFrame, Difference, original;
const char* thresh_window = "Thresh";
int threshold_value = 0;
int threshold_type = 3;
//int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst, roi;
char* window_name = "Threshold Demo";

		/// used in color set

int minHue, maxHue, minSat, maxSat, minValue, maxValue;
int minHueRed, maxHueRed, minSatRed, maxSatRed, minValueRed, maxValueRed;
int minHueBlue, maxHueBlue, minSatBlue, maxSatBlue, minValueBlue, maxValueBlue;
int minHueGreen, maxHueGreen, minSatGreen, maxSatGreen, minValueGreen, maxValueGreen;


int margin = 15; // used to set a margin of hsv
int switchColor = NULL;// used in color set swtich statement
bool colorFound = false; // used to detect certain colors only when a color is defined for inrange();

time_t timer; // not used yet
Mat thresh_callback(Mat input);
Mat colorExtraction(Mat input);
Mat Threshold_Demo(Mat input);
void checkForHand(Mat frame);
void color1(Mat image);

//void HSVtoRGB(int H, double S, double V, int output[3]);

int main() {

	Camera* cam = new Camera(1);

	cam->Initialize();

	cam->SetROI(0, 0, 1152, 964);

	roi = cam->getFrame();

	checkForHand(camFrame);

	urMom = false;
	urMom1 = false;

	namedWindow(window_capture_name, WINDOW_NORMAL);
	namedWindow(window_detection_name, WINDOW_NORMAL);
	namedWindow(difference, WINDOW_AUTOSIZE);	

	while (true) {

		camFrame = cam->getFrame();

		//flip(camFrame, camFrame, -1);

		if (urMom) {

			original = camFrame;

			imshow(window_capture_name, camFrame);
		
			urMom = false;
		}

		if (urMom1) {
			newFrame = camFrame;
			

			
			absdiff(Threshold_Demo(original), Threshold_Demo(newFrame), Difference);
			
			imshow(difference, Difference);
			thresh_callback(Difference);

			urMom1 = false;
		}


		if (urMom1 == false && urMom == false && colorFound == true) {
			Mat source = camFrame;

			colorExtraction(source);
			thresh_callback(source);

			colorFound = false;

		}
		
		int keyCode = waitKey(30);


		if (keyCode == 'v') {
			urMom1 = true;
		}


		if (keyCode == 'b') {
			urMom = true;
		}
		else if(keyCode == 'q')
		{
			break;
		}
			
	}

	cam->Close();


	return 0;
}


Mat Threshold_Demo(Mat input)
{
	
	/* 0: Binary
	   1: Binary Inverted
	   2: Threshold Truncated
	   3: Threshold to Zero
	   4: Threshold to Zero Inverted
	 */



	cvtColor(input, input, COLOR_BGR2GRAY);
	blur(input, input, Size(5, 5));


	 threshold(input, input, low_thresh, max_thresh1, THRESH_BINARY);
	 //dilate(image_gray, image_gray, 0);
	namedWindow(thresh_window, WINDOW_AUTOSIZE);
	createTrackbar("Low H", thresh_window, &low_thresh, max_thresh1);

	imshow(thresh_window, input);
	
	return input;

}


const clock_t begin_time = clock();
float time12;
float time13;


void checkForHand(Mat frame) {

	// do something
	time12 = float(clock() - begin_time) / CLOCKS_PER_SEC;
	float time13 = time12;

	while (true) {
		
		

			
			if (time13 + 5.0 < time12) {

				float time13 = time12;

				//std::cout << float(clock() - begin_time) / CLOCKS_PER_SEC;

				cout << time12 << endl;
			}

		
		time12 = float(clock() - begin_time) / CLOCKS_PER_SEC;
	}




	
}

Mat thresh_callback(Mat input)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	/// Find contours
	findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//cout << "size of contours"+contours.size() << endl;

	vector< vector<Point> > hull(contours.size());

	//

	/// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}

	///  Get the mass centers:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}

	for (int i = 0; i < contours.size(); i++)
		convexHull(Mat(contours[i]), hull[i], false);

	/// Draw contours

	for (int i = 0; i < contours.size(); i++)
	{

		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));



		double area1 = boundingRect(contours[i]).width * boundingRect(contours[i]).height; // are of the blob
		//cout << contourArea(contours[i], false) << endl;
		//by area
		bounding_rect = boundingRect(contours[i]); 


		//double size1 = contourArea(contours[i], true);
		double MinAreaLimit = 600.0;				// blob limits
		double MaxiAreaLimit = 13000.0;//
		if (area1 > MinAreaLimit && MaxiAreaLimit < 13000)  {				//&& area1<10000  && boundingRect(contours[i-1]).x != boundingRect(contours[i]).x && boundingRect(contours[i-1]).y != boundingRect(contours[i]).y

			//for (int i = 0; i < contours.size(); i++) {
			bounding_rect = boundingRect(contours[i]);

			cout << " contour number " << endl;
			cout << i << endl;
			//cout << "size of contour Area" << endl;
			cout << area1 << endl;
			double x, y;


			// center of contour 
			x = boundingRect(contours[i]).x + boundingRect(contours[i]).width / 2;
			y = boundingRect(contours[i]).y + boundingRect(contours[i]).height / 2;

			area0 = Point(x, y);// the middle of the bounding rectangle

			// draws rectangle on the bounding rectangle area
			rectangle(frame_threshold, bounding_rect, Scalar(255, 255, 255), 1, 8, 0);
						


			//region of interest set to the bounding rectangle of the blob

			Mat Roi = camFrame(bounding_rect); 
			rectangle(camFrame, bounding_rect, Scalar(255, 255, 255), 1, 8, 0);

		const char* source_window = "Source";
			color1(Roi);
			namedWindow(source_window);
			imshow(source_window, Roi);
			Roi.release();

			// draws a circle in the middle of the blob
			circle(frame_threshold, area0, 4,color, -1, 8, 0);
			//string s = to_string(i);


			// defines if the blob is vertical or horizontal or square(which is almost never)

			if (boundingRect(contours[i]).width > boundingRect(contours[i]).height) {
				putText(frame_threshold,
					"hor",
					cv::Point(area0), // Coordinates
					cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
					1.0, // Scale. 2.0 = 2x bigger
					cv::Scalar(255, 0, 0), // BGR Color
					1);

			}
			else if (boundingRect(contours[i]).width < boundingRect(contours[i]).height) {

				putText(frame_threshold,
					"ver",
					cv::Point(area0), // Coordinates
					cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
					1.0, // Scale. 2.0 = 2x bigger
					cv::Scalar(255, 255, 255), // BGR Color
					1);



			}
			else {
				putText(frame_threshold,
					"square",
					cv::Point(area0), // Coordinates
					cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
					1.0, // Scale. 2.0 = 2x bigger
					cv::Scalar(255, 0, 0), // BGR Color
					1);



			}

			// Anti-alias (Optional)
	   //}
			area1 = 0.0; // resets area of the blob

		}
		/// Show in a window
		namedWindow("Contours", WINDOW_AUTOSIZE);
		imshow("Contours", frame_threshold);

		/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
		//printf("\t Info: Area and Contour Length \n");

	}
	return input;
}



void color1(Mat image) {
	Mat image_hsv;

	cvtColor(image, image_hsv, COLOR_BGR2HSV);

	// Quanta Ratio
	int scale = 10;

	int hbins = 36, sbins = 25, vbins = 25;
	int histSize[] = { hbins, sbins, vbins };

	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	float vranges[] = { 0, 256 };

	const float* ranges[] = { hranges, sranges, vranges };
	MatND hist;

	int channels[] = { 0, 1, 2 };

	calcHist(&image_hsv, 1, channels, Mat(), // do not use mask
		hist, 3, histSize, ranges,
		true, // the histogram is uniform
		false);

	int maxVal = 0;

	int hue = 0;
	int saturation = 0;
	int value = 0;

	for (int h = 0; h < hbins; h++)
		for (int s = 0; s < sbins; s++)
			for (int v = 0; v < vbins; v++)
			{
				int binVal = hist.at<int>(h, s, v);
				if (binVal > maxVal)
				{
					maxVal = binVal;

					hue = h;
					saturation = s;
					value = v;
				}
			}

	hue = hue * scale; // angle 0 - 360
	saturation = saturation * scale; // 0 - 255
	value = value * scale; // 0 - 255

	if (hue < 155 && hue >135 && saturation > 115 && saturation < 135) {

		putText(frame_threshold,
			"Green",
			cv::Point(area0), // Coordinates
			cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			1.0, // Scale. 2.0 = 2x bigger
			cv::Scalar(255, 255, 255), // BGR Color
			1);


	}
	else if (hue < 230 && hue >210 && saturation > 150 && saturation < 175) {


		putText(frame_threshold,
			"Blue",
			cv::Point(area0), // Coordinates
			cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			1.0, // Scale. 2.0 = 2x bigger
			cv::Scalar(255, 255, 255), // BGR Color
			1);


	}



	
	if (hue < 50 && hue > 20) {
		switchColor = 0;

	}
	else if (hue < 50 && hue > 20) {

		switchColor = 1;

	}
	else if (hue < 50 && hue > 20) {

		switchColor = 2;

	}
	
		//Sets the values of the colors
	switch (switchColor) {

	case 0:
		maxHueRed = hue + margin;
		minHueRed = hue - margin;
		maxSatRed = saturation + margin;
		minSatRed = saturation - margin;
		maxValueRed = value + margin;
		minValueRed = value - margin;

		cout << "color is red" << endl;
		colorFound = false;
		break;

	case 1:
		maxHueGreen = hue + margin;
		minHueGreen = hue - margin;
		maxSatGreen = saturation + margin;
		minSatGreen = saturation - margin;
		maxValueGreen = value + margin;
		minValueGreen = value - margin;

		cout << "color is green" << endl;
		colorFound = false;
		break;

	case 2:
		maxHueBlue = hue + margin;
		minHueBlue = hue - margin;
		maxSatBlue = saturation + margin;
		minSatBlue = saturation - margin;
		maxValueBlue = value + margin;
		minValueBlue = value - margin;

		cout << "colort is blue" << endl;
		colorFound = false;
		break;

	default:
		cout << "color not found" << endl;
		colorFound = false;

		break;


	}




	cout << hue << "hue" << endl;
	cout << saturation << "saturation" << endl;
	cout << value << "value" << endl;
}



Mat colorExtraction(Mat input) {

	
	 cvtColor(input, input, COLOR_BGR2HSV);

	inRange(input,
        Scalar(minHue, 0, 0),
        Scalar(maxHue, maxSat, maxValue),
        input);




	return input;
}