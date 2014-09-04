#include <iostream>
#include <opencv2/opencv.hpp>
#include "check_scarf.h"


CheckScarf::CheckScarf()
{
	Marker_Centroid.U = -1;
	Marker_Centroid.V = -1;
	Dist_MS = -1;
}


CheckScarf::~CheckScarf()
{
}


void CheckScarf::capture_image()
{

	// Capture the video from web cam
	cv::VideoCapture cap(0);

	// If not success, exit program
	while (!cap.isOpened())
		std::cout << "Cannot open the web cam" << std::endl;

	// Read a new frame from video
	while (!cap.read(Img_rgb))
		std::cout << "Cannot read a frame from video stream" << std::endl;

}


void CheckScarf::image_color_segmentation(int const HSV[6],
			cv::Mat & imgThresholded)
{

	cv::Mat imgHSV;

	// Convert the captured frame from BGR to HSV
	cv::cvtColor(Img_rgb, imgHSV, cv::COLOR_BGR2HSV);

	// Threshold the image
	cv::inRange(imgHSV, cv::Scalar(HSV[0], HSV[2], HSV[4]),
		    cv::Scalar(HSV[1], HSV[3], HSV[5]), imgThresholded);

	// Morphological opening (remove small objects from the foreground)
	cv::erode(imgThresholded, imgThresholded,
		  getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::dilate(imgThresholded, imgThresholded,
		   getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Morphological closing (fill small holes in the foreground)
	cv::dilate(imgThresholded, imgThresholded,
		   getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::erode(imgThresholded, imgThresholded,
		  getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

}


void calc_centroid(cv::Mat & I, int &Cu, int &Cv)
{

	/// Calc moments of the image to find centroid. 
	cv::Moments ObjMoments = cv::moments(I);
	//Position of the cropped image, so offset added
	Cu = (int)(ObjMoments.m10 / ObjMoments.m00);	//U 
	Cv = (int)(ObjMoments.m01 / ObjMoments.m00);	//V 

	cv::circle(I, cv::Point(Cu, Cv), 5, cv::Scalar(128, 128, 128), 2, 8, 0);

}


int calc_mark_scarf_distance(cv::Mat & I_M, cv::Mat & I_S, int &Cu, int &Cv)
{
	calc_centroid(I_M, Cu, Cv);

	int nRows = I_S.rows;
	//int channels = I_S.channels();
	//int nCols = I_S.cols * channels;

	int i = Cv;
	bool trobat = false;
	uchar *p;
	while (i < nRows and ! trobat) {
		p = I_S.ptr < uchar > (i);
		trobat = p[Cu] == 255;
		i++;
	}

	//Return distance or -1 if scarf is not found
	if (i == nRows)
		return -1;
	else
		return i - Cv;

}


int CheckScarf::check_scarf()
{
	// Colors definition 
	int const Colors_HSV_Detect[2][6] = {	//LowH HighH LowS HighS LowV HighV
		149, 179, 0, 255, 160, 255,	//Marker Fucsia 
		25, 45, 100, 255, 40, 200	//Scarf Green 
	};

	// Capturing camera image 
	capture_image();

	// Color segmentation
	image_color_segmentation(Colors_HSV_Detect[0], Img_thres_Marker);
	image_color_segmentation(Colors_HSV_Detect[1], Img_thres_Scarf);

	// Vertical Marker-Scarf distance
	Dist_MS =
	    calc_mark_scarf_distance(Img_thres_Marker, Img_thres_Scarf,
				     Marker_Centroid.U, Marker_Centroid.V);

	return Dist_MS;

}


void CheckScarf::draw_info()
{
	/// Little bit of drawing to have some feedback
	// Distance Mark to Scarf
	cv::line(Img_rgb, cv::Point(Marker_Centroid.U, Marker_Centroid.V),
		 cv::Point(Marker_Centroid.U, Marker_Centroid.V + Dist_MS), 0,
		 2, 8, 0);
	std::stringstream ss;
	ss << Dist_MS << "px";
	putText(Img_rgb, ss.str(),
		cv::Point(Marker_Centroid.U + 5,
			  Marker_Centroid.V + Dist_MS / 2),
		cv::FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);

	// Show image thresholded 
	cv::imshow("Thresholded Image Mark", Img_thres_Marker);
	cv::imshow("Thresholded Image Polo", Img_thres_Scarf);

	// Show Original image with drawing
	cv::imshow("Original", Img_rgb);

}


int CheckScarf::wait_any_key()
{
	std::cout << "Press any key to exit" << std::endl;
	cv::waitKey(0);
	return 0;
}


int CheckScarf::wait()
{
	cv::waitKey(2000);
	return 0;
}


