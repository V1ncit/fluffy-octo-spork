#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>


using namespace cv;
using namespace std;



///////////////////  Face detection  //////////////////////

void main() {

	string path = "Resources/test.mp4";
	VideoCapture cap(path);
	Mat img;
	

	CascadeClassifier faceCascade;
	faceCascade.load("Resources/haarcascade_frontalface_default.xml");

	if (faceCascade.empty()) { cout << "XML file not loaded" << endl; }

	while (true) {

		cap.read(img);
		//resize(img, img, Size(), 0.5, 0.5);

		vector<Rect> faces;
		faceCascade.detectMultiScale(img, faces, 1.1, 10);

		if (img.empty()) {        // Break if no frame
			break;
		}

		for (int i = 0; i < faces.size(); i++) {

			rectangle(img, faces[i].tl(), faces[i].br(), Scalar(0, 255, 0), 2);

		}
		imshow("Image", img);
		char c = (char)waitKey(10);
		if (c == 27) {            // Break if user press escape
			break;
		}

	}
	destroyAllWindows();
}