#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef _CHECK_SCARF_H
#define _CHECK_SCARF_H

class CheckScarf {
 private:

	struct Centroid {
		int U;
		int V;
	};

	Centroid Marker_Centroid;
	//Distance between marker and scarf
	int Dist_MS;

	//Images
	 cv::Mat Img_rgb;
	 cv::Mat Img_thres_Marker;
	 cv::Mat Img_thres_Scarf;

	//Bools to know if we have elements
	bool There_Is_Marker;
	bool There_Is_Scarf;
	bool Img_Refreshed;

	//Functions
	void capture_image();
	void image_color_segmentation(int const HSV[6],
				      cv::Mat & imgThresholded);

 public:

	 CheckScarf();
	~CheckScarf();
	int check_scarf();
	void draw_info();
	int wait_any_key();
	int wait(int ms);

};

#endif
