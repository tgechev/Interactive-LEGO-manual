#include "Camera.h"

using namespace std;
using namespace cv;

int main() {

	Camera* cam = new Camera(1);

	cam->Initialize();

	Mat camFrame;

	namedWindow("Feed", WINDOW_NORMAL);

	while (true) {

		camFrame = cam->getFrame();

		imshow("Feed", camFrame);

		if (waitKey(30) >= 0)
			break;
	}

	cam->Close();


	return 0;
}