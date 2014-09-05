#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#ifndef _CHECK_SCARF_H
#define _CHECK_SCARF_H

class CheckScarf {
 private:

	struct Centroid {
		int U;
		int V;
	};

	// Markers centroid
	Centroid Marker_Centroid;
	// Distance between marker and scarf
	int Dist_MS;
	// Neck Area
	 cv::Rect Neck_Rect;

	// Images
	 cv::Mat Img_rgb;
	 cv::Mat Img_thres_Marker;
	 cv::Mat Img_thres_Scarf;

	// Bools to know if we have elements
	bool There_Is_Marker;
	bool There_Is_Scarf;
	bool Scarf_around_Neck;
	bool Img_Refreshed;

	// Functions
	void capture_image();
	void image_color_segmentation(int const HSV[6],
				      cv::Mat & imgThresholded);
	bool is_scarf_around_neck();

 public:

	 CheckScarf();
	~CheckScarf();
	void check_scarf(std::vector<int> &v);
	void draw_info();
	int wait_any_key();
	int wait(int ms);

};

#endif
