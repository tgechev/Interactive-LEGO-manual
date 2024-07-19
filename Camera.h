#ifndef CAMERA_H
#define CAMERA_H
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <string>
#include <uEye.h>
#include<mutex>

//Header file of the simple wrapper Camera class which either uses the uEye camera and its API if it is connected
//or a simple VideoCapture for the built in laptop camera

class Camera {
private:
    HIDS hCam;                     //Camera handle - used by the uEye API to identify the camera used
    cv::VideoCapture camStream;    //VideoCapture object, used if no uEye camera is connected
    std::mutex myMutex;            //Mutex needed for locking the code responsible for capturing a frame

	//image mem vars
    char* pMem = NULL;             //Pointer to the memory of the frame being captured
    int memID = 0;                 //ID of the memory of the frame

    int width;                     //Width of frame
    int height;                    //Height of frame

    //Public members and functions
public:
    Camera(UINT _devID);  //Camera class constructor, which requires a UINT variable holding the ID of the camera
    void Initialize();    //A function which initiallizes the camera
    void SetROI(INT _X, INT _Y, INT _roiWidth, INT _roiHeight);     //Function which sets a particular Region of Interest
                                                                    //with assigned (x, y) coordinates and (w, h) size,
                                                                    //resulting in a frame with the ROI's properties

    cv::Mat getFrame();   //A function which retrieves a frame from the camera

    void Close();        //A function which explicitly closes the connection to the camera
};
#endif
