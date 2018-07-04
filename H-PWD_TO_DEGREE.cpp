#include "pca9685_control.h"
#include "opencv2/opencv.hpp"
#include <pthread.h>
#include <iostream>

//using namespace
using namespace cv;
using namespace std;

//Á¤ÀÇ
void* Moter_Function(void*);
void* Image_Function(void*);
Mat Rrotation(int ag);

//global
int val;

//MAIN
int main(void)
{
	pthread_t moter_thread,image_thread;
	void * thread_return;
	wiringPiSetup();
	int rtnNum=0;
	rtnNum=Initulize();
	
	if(rtnNum==-1)return -1;
    pca9685_resetart();
    pca9685_freq();

	pthread_create(&moter_thread, NULL, Moter_Function,NULL);
	pthread_create(&image_thread, NULL, Image_Function,NULL);
	pthread_detach(moter_thread);
	while(1){
	printf("val is :%d\n",val);
	scanf("%d", &val);
	}
    return 0;
}

void* Moter_Function(void *){
	unsigned short time_val = 4095;
	char key;
	int cnt = 1;
	while (1)
	{
		reg_write16(LED15_ON_L, time_val - val);
		reg_read16(LED15_ON_L);
		reg_write16(LED15_OFF_L, time_val);
		reg_read16(LED15_OFF_L);

	}
	return NULL;
}

void* Image_Function(void*){
	Mat  result, gray_frame,img;
	double minVal, maxVal;
	int degree=0;
	Point minLoc, maxLoc, matchLoc;
	VideoCapture cap(0);
	Mat  templ,temp2;
	templ = imread("test3.jpg", 1);
	Mat img_display;
	int result_cols ;
	int result_rows;
	while(1){
	cap>>img ;	
	img.copyTo(img_display);
	imshow("Source Image", img_display);

	result_cols = img.cols - templ.cols + 1;
	result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_8UC3);

	inRange(img_display, Scalar(200, 200, 200), Scalar(255, 255, 255), gray_frame);
	imshow("Gray Image", gray_frame);
	temp2=Rrotation(degree);
	matchTemplate(img, temp2, result, TM_CCOEFF_NORMED);

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	matchLoc = maxLoc;

	if(maxVal > 0.8) {
	printf("minVal : %lf  maxVal : %lf degre:%d", minVal, maxVal,degree);
	cout << "Point minLoc : " << minLoc << "Point maxLoc : " << maxLoc << endl;
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(255), 2, 8, 0);
	}

	

	imshow("Source Image", img_display);
	imshow("template Image", templ);
	
	}
}
Mat Rrotation(int ag) {
	// Load the image
	Mat imgOriginal = imread("test3.jpg", 1);

	//show the original image
	const char* pzOriginalImage = "Original Image";
	namedWindow(pzOriginalImage, CV_WINDOW_AUTOSIZE);
	imshow(pzOriginalImage, imgOriginal);

	const char* pzRotatedImage = "Rotated Image";
	namedWindow(pzRotatedImage, CV_WINDOW_AUTOSIZE);

	int iImageHieght = imgOriginal.rows / 2;
	int iImageWidth = imgOriginal.cols / 2;


	Mat matRotation = getRotationMatrix2D(Point(iImageWidth, iImageHieght), ag, 1);

	// Rotate the image
	Mat imgRotated;
	warpAffine(imgOriginal, imgRotated, matRotation, imgOriginal.size());

	imshow(pzRotatedImage, imgRotated);

	return imgRotated;

}