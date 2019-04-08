#include "opencv2/core/core.hpp"	
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

//http://raphael.candelier.fr/?blog=Image%20Moments object orientation


using namespace std;
using namespace cv;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
Mat image, image_gray;
int largest_area = 0;
int largest_contour_index = 0;
Rect bounding_rect;


 int low_thresh = 100;
 int max_thresh1 = 255;



void thresh_callback(int, void*);
void Threshold_Demo(int, void*);

const char* thresh_window = "Thresh";

int main(int argc, char** argv)
{



	image = imread("Image.png", IMREAD_COLOR);

	if (!image.data)  // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	
	

	cvtColor(image, image_gray, COLOR_BGR2GRAY);
	blur(image_gray, image_gray, Size(3, 3));
	const char* source_window = "Source";
	namedWindow(source_window);
	imshow(source_window, image);
	erode(image_gray,image_gray,2);
	//xThreshold_Demo(0, 0);


	createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback);
	thresh_callback(0, 0);
	
		waitKey(0); // Wait for a keystroke in the window
		return 0;

}
void Threshold_Demo(int, void*)
{
	/* 0: Binary
	   1: Binary Inverted
	   2: Threshold Truncated
	   3: Threshold to Zero
	   4: Threshold to Zero Inverted
	 */
	
	//threshold(image_gray, image_gray, low_thresh, max_thresh1, THRESH_BINARY);
	//dilate(image_gray, image_gray, 0);
	namedWindow(thresh_window, WINDOW_AUTOSIZE);
	createTrackbar("Low H", thresh_window, &low_thresh, max_thresh1, Threshold_Demo);
	
	imshow(thresh_window, image_gray);
	
}


int bb=0;



void thresh_callback(int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(image_gray, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//bb = contours.size();
	vector< vector<Point> > hull(contours.size());

	//

	//for (int i = 0; i < fifthcontour.size(); i++) {
		//Point coordinate_i_ofcontour = fifthcontour.size();
		//cout << endl << "contour with coordinates: x = " << coordinate_i_ofcontour.x << " y = " << coordinate_i_ofcontour.y;
	//}



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
	


	//cout << "Could not open or find the image" << std::endl;
	//float distance;
	float totalX = 0.0, totalY = 0.0;
	for (int i = 0; i < contours.size(); i++) {
		totalX += mc[i].x;
		totalY += mc[i].y;
		
		


		
	}


	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	//cout << totalX << endl;
	//cout << totalY << endl;

	//Point2f massCenter(totalX / contours.size() , totalY / contours.size() );
	Point2f massCenter(mc[1].x, mc[1].y);

	//cout << mc[1].x << endl;
	//cout << mc[1].y << endl;
	//cout << mc[4].x << endl;
	//cout << mc[4].y << endl;
	float someXVal = mc[4].x;
	float someYVal = mc[4].y;
	Point2f someOtherPoint(someXVal, someYVal);
//	cout << massCenter << endl;
	Point2f distance = massCenter-someOtherPoint;

	//cout << distance << endl;
	
	//cout << mu.size() << endl;

	//Point2f massCenter(totalX / mu.size(), totalY / mu.size());  
													// condition: size != 0
	//Point2f someOtherPoint(someXVal, someYVal);

	//distance = massCenter.distance(someOtherPoint);


	/// Draw contours
	
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		double a = contourArea(contours[i], false);
		cout << contourArea(contours[i], false) << endl;
		//by area
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;    //Store the index of largest contour
			
			
			bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}
		if (contourArea(contours[i], false) > 10) {

			drawContours(drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, CHAIN_APPROX_SIMPLE, Point());
			rectangle(drawing, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

			// draw a line between 2 center points
			line(drawing, Point(0, 0), Point(mc[4].x, mc[4].y), Scalar(110, 220, 0), 2, 8);
			circle(drawing, mc[i], 4, color, -1, 8, 0);
		}
	}
	
	//Mat gray, thresh;
	//cv :: Mat im = imread("Image.png", CV_8UC1);
	//cvtColor(im, gray, COLOR_BGR2GRAY);
	//threshold(gray,thresh, 128, 255, THRESH_BINARY);







	//double huMoments[7]; 
	//Moments mom=moments(contours[0]);
	//HuMoments(mom, huMoments);

	/*for (int i = 0; i < 7; i++)
	{
		huMoments[i] = -1 * copysign(1.0, huMoments[i]) * log10(abs(huMoments[i]));

		//printf("%f\n",huMoments[i]);
	}*/
	

	/// Show in a window
	namedWindow("Contours", WINDOW_AUTOSIZE);
	imshow("Contours", drawing);

	/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
	//printf("\t Info: Area and Contour Length \n");
	for (int i = 0; i < contours.size(); i++)
	{
		//printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n", i, mu[i].m00, contourArea(contours[i]), arcLength(contours[i], true));
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 8, 0);
	}


}
