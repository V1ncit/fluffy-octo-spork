#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>


using namespace cv;
using namespace std;


Mat imgOriginal, imgGray, imgCanny, imgResize, imgThre, imgBlur, imgDil, imgErode, imgWarp, imgCrop;
vector<Point> initialPoints, docPoints;

// A4 paper size in pixels
float w = 420, h = 570;


// Preprocessing
Mat preProcessing(Mat img) {

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(5, 5), 5, 0);
	Canny(imgBlur, imgCanny, 50, 150);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	return imgDil;
}


// Detects contours with four edges
vector<Point> getContours(Mat img) {


	vector<vector<Point>> contours;
	vector<Vec4i> hirarchy;

	findContours(imgDil, contours, hirarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	vector<Point> biggest;
	int maxArea = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;


		if (area > 50000)
		{
			float pari = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * pari, true);

			if (area > maxArea && conPoly[i].size() == 4 ){

				//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
				biggest = { conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3] };
				maxArea = area;

			}

			drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
			//rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);

		}

	}
	return biggest;
}

// Marking corners with dots and text
void drawPoints(vector<Point> points, Scalar color) {

	for (int i = 0; i < points.size(); i++) {

		circle(imgOriginal, points[i], 10, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 2, color, 2);


	}

}

// To rearrange corners for warping
vector<Point> reorder(vector<Point> points) {

	vector<Point> newPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++) {

		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);

	}
	
	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);

	return newPoints;
}

// Wapres the image
Mat getWarped(Mat img, vector<Point> points, float w, float h) {

	Point2f src[4] = { points[0], points[1], points[2], points[3]};
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));


	return imgWarp;

}


// main function
void main() {


	string path = "Resources/blad.jpg";
	imgOriginal = imread(path);

	// Temp scaling
	resize(imgOriginal, imgOriginal, Size(), 0.2, 0.2);


	// Preprocessing

	imgThre = preProcessing(imgOriginal);

	// Get Contours

	initialPoints = getContours(imgThre);

	// drawPoints(initialPoints, Scalar(0,0,255));

	docPoints = reorder(initialPoints);
	//drawPoints(docPoints, Scalar(0, 0, 255));
	
	// Warp
	imgWarp = getWarped(imgOriginal, docPoints, w, h);


	// Crop
	int cropVal = 5;
	Rect roi(cropVal, cropVal, w - (2*cropVal), h - (2*cropVal));
	imgCrop = imgWarp(roi);
	

	imshow("Image", imgOriginal);
	imshow("Image Gray", imgGray);
	imshow("Image Blur", imgBlur);
	imshow("Image Canny", imgCanny);
	imshow("Image Dilation", imgDil);
	imshow("Document", imgWarp);
	imshow("Image Cropped", imgCrop);

	waitKey(0);

}