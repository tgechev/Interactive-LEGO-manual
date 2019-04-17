#include "opencv2/core/core.hpp"	
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <string> 
#include <ctime>

//http://raphael.candelier.fr/?blog=Image%20Moments object orientation


using namespace std;
using namespace cv;

Mat GridCheckingsAndSuch(Mat dispImage_mat);
Mat CameraGrid(Mat grid_mat);
Mat SquareGrid(Mat grid_mat);


int thresh = 255;
int max_thresh = 255;
RNG rng(12345);
Mat image, image_gray;
int largest_area = 0;
int largest_contour_index = 0;
Rect bounding_rect;


 int low_thresh = 100;
 int max_thresh1 = 255;


 //dialation 

 Mat src, erosion_dst, dilation_dst;
 int erosion_elem = 0;
 int erosion_size = 0;
 int dilation_elem = 0;
 int dilation_size = 0;
 int const max_elem = 2;
 int const max_kernel_size = 21;
 void Erosion(int, void*);
 void Dilation(int, void*);

void thresh_callback(int, void*);
void Threshold_Demo(int, void*);

const char* thresh_window = "Thresh";

int main(int argc, char** argv)
{



	image = imread("Image6.png", IMREAD_COLOR);

	if (!image.data)  // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}







	

	namedWindow("Erosion Demo", WINDOW_AUTOSIZE);
	namedWindow("Dilation Demo", WINDOW_AUTOSIZE);
	moveWindow("Dilation Demo", image.cols, 0);



	namedWindow("Erosion Demo", WINDOW_AUTOSIZE);
	namedWindow("Dilation Demo", WINDOW_AUTOSIZE);
	moveWindow("Dilation Demo", image.cols, 0);
	createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo",
		&erosion_elem, max_elem,
		Erosion);
	createTrackbar("Kernel size:\n 2n +1", "Erosion Demo",
		&erosion_size, max_kernel_size,
		Erosion);
	createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
		&dilation_elem, max_elem,
		Dilation);
	createTrackbar("Kernel size:\n 2n +1", "Dilation Demo",
		&dilation_size, max_kernel_size,
		Dilation);

	

	cvtColor(image, image_gray, COLOR_BGR2GRAY);

	//bilateralFilter(image_gray, image_gray,3, 75, 75);
	GaussianBlur(image_gray, image_gray, Size(5, 5), 0, 0);
	
	const char* source_window = "Source";
	namedWindow(source_window);
	//dilate(image_gray, image_gray, 3);
	

	//xThreshold_Demo(0, 0);
	//Mat cameraGrid = CameraGrid(image_gray);

	Mat squareGrid = SquareGrid(image_gray);


	namedWindow("Contours", WINDOW_AUTOSIZE);
	imshow("Contours", image_gray);

	createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback);


	Erosion(0, 0);
	Dilation(0, 0);
	


	//thresh_callback(0, 0);
	
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
	findContours(canny_output, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//cout << "size of contours"+contours.size() << endl;



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
		
		
		
		double area1 = boundingRect(contours[i]).width * boundingRect(contours[i]).height;
		//cout << contourArea(contours[i], false) << endl;
		//by area
		bounding_rect = boundingRect(contours[i]);

		




		//double size1 = contourArea(contours[i], true);
		double limit = 600.0;
		if (area1>limit ){				//  && boundingRect(contours[i-1]).x != boundingRect(contours[i]).x && boundingRect(contours[i-1]).y != boundingRect(contours[i]).y

			//for (int i = 0; i < contours.size(); i++) {
				bounding_rect = boundingRect(contours[i]);
				
				cout << " contour number " << endl;
				cout << i << endl;
				cout << "size of contour Area" << endl;
				cout << area1 << endl;
				double x, y;
			
					x = boundingRect(contours[i]).x + boundingRect(contours[i]).width / 2;
					y = boundingRect(contours[i]).y + boundingRect(contours[i]).height / 2;

				
				

				Point area0 = Point(x, y);



				rectangle(drawing, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
				circle(drawing, area0, 4, color, -1, 8, 0);
				string s = to_string(i);

				//	putText(drawing,
				//	s,
				//	cv::Point(area0), // Coordinates
				//	cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
				//	1.0, // Scale. 2.0 = 2x bigger
				//	cv::Scalar(255, 255, 255), // BGR Color
				//	1);

					if (boundingRect(contours[i]).width > boundingRect(contours[i]).height) {
						putText(drawing,
							"hor",
							cv::Point(area0), // Coordinates
							cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
							1.0, // Scale. 2.0 = 2x bigger
							cv::Scalar(255, 255, 255), // BGR Color
							1);

					}
					else if (boundingRect(contours[i]).width < boundingRect(contours[i]).height) {

						putText(drawing,
							"ver",
							cv::Point(area0), // Coordinates
							cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
							1.0, // Scale. 2.0 = 2x bigger
							cv::Scalar(255, 255, 255), // BGR Color
							1);



					}
					else {
						putText(drawing,
							"square",
							cv::Point(area0), // Coordinates
							cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
							1.0, // Scale. 2.0 = 2x bigger
							cv::Scalar(255, 255, 255), // BGR Color
							1);



					}

				 // Anti-alias (Optional)
			//}
				area1 = 0.0;

		}



		


		//bounding_rect = boundingRect(contours[i]);
			//drawContours(drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, CHAIN_APPROX_SIMPLE, Point());
			

			// draw a line between 2 center points
			//line(drawing, Point(0, 0), Point(mc[4].x, mc[4].y), Scalar(110, 220, 0), 2, 8);
			
		//}
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
	


}

void Erosion(int, void*)
{
	int erosion_type = 0;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(image_gray, image_gray, element);
	imshow("Erosion Demo", image_gray);
}
void Dilation(int, void*)
{
	int dilation_type = 0;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	dilate(image_gray, image_gray, element);
	imshow("Dilation Demo", image_gray);
}

int gridSize_i = 40;
Mat CameraGrid(Mat gridinput_mat)
{
	int width_i = gridinput_mat.rows;  // the width of the grid matches the rows(all pixels in horizontal direction )
	int height_i = gridinput_mat.cols;
	int stepSize_i = 24; // calculating how many squares the grid will have 

	 cout << stepSize_i << endl;

	for (int i = 0; i <= width_i; i++) // drawing horizontal lines 
		line(gridinput_mat, Point(6*stepSize_i, i * stepSize_i), Point(gridSize_i * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));

	for (int i = 0; i <= height_i; i++)// drawing vertical lines 
		line(gridinput_mat, Point(i * stepSize_i, 6*stepSize_i), Point(i * stepSize_i, gridSize_i * stepSize_i), Scalar(255, 255, 255));

	return gridinput_mat;
}

Mat SquareGrid(Mat gridinput_mat)
{
	int width_i = gridinput_mat.rows;  // the width of the grid matches the rows(all pixels in horizontal direction )
	int height_i = gridinput_mat.cols;
	int stepSize_i = 24; // calculating how many squares the grid will have 

	cout << stepSize_i << endl;

	for (int i = 0; i <= width_i; i++) // drawing horizontal lines 

		rectangle(gridinput_mat, Point(6 * stepSize_i, i * stepSize_i), Point(gridSize_i * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));


		//line(gridinput_mat, Point(6 * stepSize_i, i * stepSize_i), Point(gridSize_i * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));

	for (int i = 0; i <= height_i; i++)// drawing vertical lines 
		//line(gridinput_mat, Point(i * stepSize_i, 6 * stepSize_i), Point(i * stepSize_i, gridSize_i * stepSize_i), Scalar(255, 255, 255));

	return gridinput_mat;
}