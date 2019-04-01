
#include "Camera.h"

void Camera::Initialize() {

	if (hCam == 0) {
		camStream.open(0);
	}
	else {

		INT nRet = is_InitCamera(&hCam, NULL);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to initialize camera." << std::endl;
		}

		else if (nRet == IS_SUCCESS) {

			nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set color mode." << std::endl;
			}

			nRet = is_SetDisplayMode(hCam, IS_SET_DM_DIB);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set display mode." << std::endl;
			}

			DOUBLE newFPS;
			nRet = is_SetFrameRate(hCam, 100, &newFPS);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set frame rate." << std::endl;
			}

			else if (nRet == IS_SUCCESS) {
				std::cout << "FPS set to: " << newFPS << std::endl;
			}

			nRet = is_SetSaturation(hCam, 130, 130);
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set saturation." << std::endl;
			}

			UINT nEdgeEnhancement = 1;
			nRet = is_EdgeEnhancement(hCam, IS_EDGE_ENHANCEMENT_CMD_SET, (void*)&nEdgeEnhancement, sizeof(nEdgeEnhancement));
			if (nRet != IS_SUCCESS) {
				std::cout << "Unable to set edge enhancement." << std::endl;
			}

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

cv::Mat Camera::getFrame() {
	cv::Mat camFrame, videoFrame;
	INT nRet;
	myMutex.lock();
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
		nRet = is_SetImageMem(hCam, pMem, memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to set image memory." << std::endl;
		}

		nRet = is_FreezeVideo(hCam, IS_WAIT);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to capture frame: " <<nRet<< std::endl;

		}
		else {
			camFrame = cv::Mat(height, width, CV_8UC3, pMem, cv::Mat::AUTO_STEP);
			videoFrame = camFrame.clone();
		}
	}

	myMutex.unlock();

	return videoFrame;
}
Camera::Camera(UINT _devID) {
	if (_devID > 0) {
		hCam = (_devID | IS_USE_DEVICE_ID);
	}
	else {
		hCam = 0;
	}
}

/*cv::VideoCapture Camera::getStream() {
	return camStream;
}*/

void Camera::Close() {
	if (camStream.isOpened()) {
		camStream.release();
	}
	else {
		INT nRet = is_FreeImageMem(hCam, pMem, memID);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to free image memory." << std::endl;
		}
		nRet = is_ExitCamera(hCam);
		if (nRet != IS_SUCCESS) {
			std::cout << "Unable to exit camera." << std::endl;
		}
	}
	
}