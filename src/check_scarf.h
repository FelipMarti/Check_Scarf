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

	// HSV Marker Color
	std::vector <int> Colors_HSV_Marker;

	// HSV Scarf Color
	std::vector <int> Colors_HSV_Scarf;

	// Marker's centroid
	Centroid Marker_Centroid;
	// Distance between marker and scarf
	int Dist_MS;
	// Distance Hanging left end scarf 
	int Dist_HLS;
	Centroid Max_Hanging_L_point;
	// Distance Hanging right end scarf
	int Dist_HRS;
	Centroid Max_Hanging_R_point;
	// Neck Area
	 cv::Rect Neck_Rect;
	// Scarf left hanging area
	 cv::Rect Left_Rect;
	// Scarf Right hanging area
	 cv::Rect Right_Rect;

	// Images
	 cv::Mat Img_rgb;
	 cv::Mat Img_thres_Marker;
	 cv::Mat Img_thres_Scarf;

	// Bools to know if we have elements
	bool There_Is_Marker;
	bool There_Is_Scarf;
	bool Scarf_around_Neck;
	bool End_Scarf_Hangs_Left;
	bool End_Scarf_Hangs_Right;
	bool Img_Refreshed;

	// Functions
	void capture_image();
	void image_color_segmentation(std::vector <int> HSV,
				      cv::Mat & imgThresholded);
	bool is_scarf_around_neck();
	bool does_scarf_end_hang(cv::Rect const &R, Centroid & C);

 public:

	 CheckScarf();
	~CheckScarf();
	void set_HSV_Color (bool const Marker, int const HSV[6]);
	void check_scarf(std::vector < int >&v);
	void draw_info();
	int wait_any_key();
	int wait(int ms);

};

#endif
