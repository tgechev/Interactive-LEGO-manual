#include "Camera.h"

using namespace std;
using namespace cv;

int main() {

	Camera* cam = new Camera(1);

	cam->Initialize();

	cam->SetROI(0, 0, 1152, 964);

	Mat camFrame;

	namedWindow("Feed", WINDOW_NORMAL);

	while (true) {

		camFrame = cam->getFrame();

		flip(camFrame, camFrame, -1);

		imshow("Feed", camFrame);

		if (waitKey(30) >= 0)
			break;
	}

	cam->Close();


	return 0;
}