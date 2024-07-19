
#include "Camera.h"

//The definition/implementation of the Initialize() function
void Camera::Initialize() {

    //If the handle to the camera is equal to 0, meaning that the used camera is the builtin webcam
    //Open a VideoCapture stream instead of using the uEye API
	if (hCam == 0) {
        camStream.open(0);
	}

    //If the camera is not the builtin webcam, then it is the uEye camera, hence execute the below code
	else {

        //A function from the uEye API, responsible for initializing the camera, it requires the address of the camera handle (&hCam)
        //and a pointer to a window, where the image will be displayed, we do not need such a window, hence the pointer is NULL
		INT nRet = is_InitCamera(&hCam, NULL);

        //is_InitCamera() returns an INT value indicating the result of execution, it is stored in nRet and is checked whether it is successfull
        //if not print out an error message, if yes - proceed.
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to initialize camera." << std::endl;
		}

        //If the camera is successfully initialized, continue setting parameters
		else if (nRet == IS_SUCCESS) {

            //Try setting the color mode to BGR8
			nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set color mode." << std::endl;
			}

            //Try setting the display mode to DIB, which captures an image and stores in System Memory (RAM)
			nRet = is_SetDisplayMode(hCam, IS_SET_DM_DIB);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set display mode." << std::endl;
			}

            //Try setting a specific frame rate.
			DOUBLE newFPS;
			nRet = is_SetFrameRate(hCam, 100, &newFPS);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set frame rate." << std::endl;
			}

			else if (nRet == IS_SUCCESS) {
				std::cout << "FPS set to: " << newFPS << std::endl;
			}

            //Try setting a specific saturation
			nRet = is_SetSaturation(hCam, 130, 130);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set saturation." << std::endl;
			}

            //Try using eddge enhancement
			UINT nEdgeEnhancement = 1;
			nRet = is_EdgeEnhancement(hCam, IS_EDGE_ENHANCEMENT_CMD_SET, (void*)&nEdgeEnhancement, sizeof(nEdgeEnhancement));
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set edge enhancement." << std::endl;
			}

            //Try setting a specific gamma
			INT nGamma = 160;
			nRet = is_Gamma(hCam, IS_GAMMA_CMD_SET, (void*)&nGamma, sizeof(nGamma));
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set Gamma" << std::endl;
			}


			DOUBLE nEnable = 1, nDummy = 0;
			//enable AGC
			nRet = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_GAIN, &nEnable, &nDummy);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set AGC." << std::endl;
			}
			//enable AES
			nRet = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SHUTTER, &nEnable, &nDummy);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set AES." << std::endl;
			}
			//Disable AFR
			nRet = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &nDummy, &nDummy);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to disable AFR." << std::endl;
			}
			//Enable AWB
			nRet = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &nEnable, &nDummy);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set AWB." << std::endl;
			}
		}
	}
}

//The definition/implementation of the SetROI() function
void Camera::SetROI(INT _X, INT _Y, INT _roiWidth, INT _roiHeight) {

    //Take the passed arguments from the function and set them to the rectAOI object
	width = _roiWidth;

	height = _roiHeight;
	
	IS_RECT rectAOI;

	rectAOI.s32X = _X;

	rectAOI.s32Y = _Y;

	rectAOI.s32Width = _roiWidth;

	rectAOI.s32Height = _roiHeight;

    //Pass the rectAOI object to the is_AOI() API function which actually does the rest
	INT nRet = is_AOI(hCam, IS_AOI_IMAGE_SET_AOI, (void*)&rectAOI, sizeof(rectAOI));

	if (nRet != IS_SUCCESS) {
		std::cout << "Unable to set ROI." << std::endl;
	}
}


cv::Mat Camera::getFrame() {
    //openCV Mat objects for storing the frame
	cv::Mat camFrame, videoFrame;

    //variable for holding function execution results
	INT nRet;

    //Uses mutex to lock below code from another thread accessing it while it is already in use by a thread
	myMutex.lock();
    //If the VideoCapture stream is opened, use it to capture the frame, if not use uEye API
	if (camStream.isOpened()) {
		camStream.read(camFrame);
		videoFrame = camFrame.clone();
	}
	else {
		// Prepare for creating image buffers

		// Allocate image mememory
		nRet = is_AllocImageMem(hCam, width, height, 24, &pMem, &memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to allocate image memory." << std::endl;
		}
        //Set image memory
		nRet = is_SetImageMem(hCam, pMem, memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to set image memory." << std::endl;
		}

        //Take the frame
		nRet = is_FreezeVideo(hCam, IS_WAIT);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to capture frame: " <<nRet<< std::endl;

		}
        //If successfull construct a Mat object and store it in camFrame
		else {
			camFrame = cv::Mat(height, width, CV_8UC3, pMem, cv::Mat::AUTO_STEP);
            //clone camFrame in videoFrame
			videoFrame = camFrame.clone();
		}

        //Free the memory for the next frame capture
		nRet = is_FreeImageMem(hCam, pMem, memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to free image memory." << std::endl;
		}
	}

	myMutex.unlock();


	return videoFrame;
}

//Camera constructor definition
Camera::Camera(UINT _devID) {
	if (_devID > 0) {
		hCam = (_devID | IS_USE_DEVICE_ID);
	}
	else {
		hCam = 0;
	}
}

//Explicitly close the camera connection
void Camera::Close() {
    //If videoCapture stream is used, release it
	if (camStream.isOpened()) {
		camStream.release();
	}

	else {
		INT nRet = is_FreeImageMem(hCam, pMem, memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to free image memory." << std::endl;
		}
        //Call uEye API function to exit the camera
		nRet = is_ExitCamera(hCam);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to exit camera." << std::endl;
		}
	}
	
}
