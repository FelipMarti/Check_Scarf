#include "check_scarf.h"

CheckScarf::CheckScarf()
{
	Marker_Centroid.U = -1;
	Marker_Centroid.V = -1;

	// Init Distances
	Dist_MS = -1;
	Dist_HLS = -1;
	Dist_HRS = -1;

	// Init bools
	There_Is_Marker = false;
	There_Is_Scarf = false;
	Scarf_around_Neck = false;
	End_Scarf_Hangs_Left = false;
	End_Scarf_Hangs_Right = false;
	Img_Refreshed = false;

	// HSV Colors
	Colors_HSV_Marker.resize(6);
	Colors_HSV_Scarf.resize(6);
}

CheckScarf::~CheckScarf()
{
}

void CheckScarf::capture_image()
{

	// Capture the video from web cam
	cv::VideoCapture cap(0);

	// Open the camera 
	while (!cap.isOpened())
		std::cout << "Cannot open the web cam" << std::endl;

	// Read a new frame from video
	while (!cap.read(Img_rgb))
		std::cout << "Cannot read a frame from video stream" <<
		    std::endl;

}

void CheckScarf::image_color_segmentation(std::vector <int> HSV,
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
}

void Check_Rect_Values(cv::Rect & R, int rows, int cols)
{
	// Check the Rect is inside of the image dimensions
	if (R.x < 0) {
		R.width = R.width + R.x;
		R.x = 0;
	}
	if (R.y < 0) {
		R.height = R.height + R.y;
		R.y = 0;
	}
	if (R.x + R.width > cols)
		R.width = cols - R.x;
	if (R.y + R.height > rows)
		R.height = rows - R.y;
}

int calc_mark_scarf_distance(cv::Mat & I_M, cv::Mat & I_S, int &Cu, int &Cv)
{
	// Calc Verical distance between marker and scarf
	int nRows = I_S.rows;
	int i = Cv;
	bool trobat = false;
	uchar *p;
	while (i < nRows and ! trobat) {
		p = I_S.ptr < uchar > (i);
		trobat = p[Cu] == 255;
		i++;
	}

	return i - Cv;

}

void CheckScarf::set_HSV_Color (bool const Marker, int const HSV[6]) {
	
	// Fuction to set marker and scarf color HSV values
	if (Marker) {
		// LowH HighH LowS HighS LowV HighV
		Colors_HSV_Marker[0]=HSV[0];
		Colors_HSV_Marker[1]=HSV[1];
		Colors_HSV_Marker[2]=HSV[2];
		Colors_HSV_Marker[3]=HSV[3];
		Colors_HSV_Marker[4]=HSV[4];
		Colors_HSV_Marker[5]=HSV[5];
	}
	else {
		// LowH HighH LowS HighS LowV HighV
		Colors_HSV_Scarf[0]=HSV[0];
		Colors_HSV_Scarf[1]=HSV[1];
		Colors_HSV_Scarf[2]=HSV[2];
		Colors_HSV_Scarf[3]=HSV[3];
		Colors_HSV_Scarf[4]=HSV[4];
		Colors_HSV_Scarf[5]=HSV[5];
	}

}

bool CheckScarf::is_scarf_around_neck()
{
	// Check if Scarf is around the neck
	int nRows = Neck_Rect.y + Neck_Rect.height;
	int channels = Img_thres_Scarf.channels();
	int nCols = (Neck_Rect.x + Neck_Rect.width) * channels;
	bool trobat = false;
	uchar *p;
	int i = Neck_Rect.y;

	// Trying to find a stright horizontal white line
	while (i < nRows and ! trobat) {
		p = Img_thres_Scarf.ptr < uchar > (i);
		int j = Neck_Rect.x;
		bool black_px = false;

		while (j < nCols and ! trobat and ! black_px) {
			if (p[j] == 0)
				black_px = true;
			//p[j] = 128;           //To Debug
			j++;
		}

		if (!black_px)
			trobat = true;
		i++;
	}

	return trobat;

}

bool CheckScarf::does_scarf_end_hang(cv::Rect const &R, Centroid & C)
{

	int nRows = R.y + R.height;
	int channels = Img_thres_Scarf.channels();
	int nCols = (R.x + R.width) * channels;
	bool trobat = false;
	int i = R.x;

	// Trying to find a stright Vertical white line
	C.U = R.x;
	C.V = R.y;
	while (i < nCols and ! trobat) {
		int j = R.y;
		bool black_px = false;

		while (j < nRows and ! trobat and ! black_px) {
			if (Img_thres_Scarf.at < uchar > (j, i) == 0)
				black_px = true;
			Img_thres_Scarf.at < uchar > (j, i) = 128;	//To Debug
			j++;
		}

		if (j - 1 > C.V) {
			C.U = i;
			C.V = j;
		}

		if (j == nRows)
			trobat = true;

		i++;
	}

	if (C.V > R.y)
		trobat = true;

	return trobat;

}

void CheckScarf::check_scarf(std::vector < int >&output)
{

	// Init output var
	output.resize(7);

	// Capturing camera image 
	capture_image();

	// Color segmentation
	image_color_segmentation(Colors_HSV_Marker, Img_thres_Marker);
	There_Is_Marker = (cv::countNonZero(Img_thres_Marker) > 0);
	image_color_segmentation(Colors_HSV_Scarf, Img_thres_Scarf);
	There_Is_Scarf = (cv::countNonZero(Img_thres_Scarf) > 0);

	// Processing Image 
	if (There_Is_Marker and There_Is_Scarf) {

		/// Calc marker centroid
		calc_centroid(Img_thres_Marker, Marker_Centroid.U,
			      Marker_Centroid.V);

		/// Checking if Scarf is around the Neck
		// Init neck ROI
		Neck_Rect.x = Marker_Centroid.U - 40;
		Neck_Rect.y = Marker_Centroid.V + 40;
		Neck_Rect.width = 80;
		Neck_Rect.height = 150;
		// Check if ROI is OK to Prevent segmentation fault
		Check_Rect_Values(Neck_Rect, Img_rgb.rows, Img_rgb.cols);
		// Checking in ROI if Scarf is found around the Neck
		Scarf_around_Neck = is_scarf_around_neck();

		// Vertical Marker-Scarf distance
		if (Scarf_around_Neck) {
			Dist_MS =
			    calc_mark_scarf_distance(Img_thres_Marker,
						     Img_thres_Scarf,
						     Marker_Centroid.U,
						     Marker_Centroid.V);
		}
		else
			Dist_MS = -1;

		//Checking if Ends of scarf hang (left)
		// Init neck ROI
		Left_Rect.x = Marker_Centroid.U - 140;
		Left_Rect.y = Marker_Centroid.V + 150;
		Left_Rect.width = 120;
		Left_Rect.height = Img_rgb.rows - Left_Rect.y;
		// Check if ROI is OK to Prevent segmentation fault
		Check_Rect_Values(Left_Rect, Img_rgb.rows, Img_rgb.cols);
		// Checking in ROI if Scarf is hanging left 
		End_Scarf_Hangs_Left =
		    does_scarf_end_hang(Left_Rect, Max_Hanging_L_point);
		if (End_Scarf_Hangs_Left)
			Dist_HLS = Max_Hanging_L_point.V - Left_Rect.y;
		else
			Dist_HLS = -1;

		//Checking if Ends of scarf hang (right)
		// Init neck ROI
		Right_Rect.x = Marker_Centroid.U + 20;
		Right_Rect.y = Marker_Centroid.V + 150;
		Right_Rect.width = 120;
		Right_Rect.height = Img_rgb.rows - Left_Rect.y;
		// Check if ROI is OK to Prevent segmentation fault
		Check_Rect_Values(Right_Rect, Img_rgb.rows, Img_rgb.cols);
		// Checking in ROI if Scarf is hanging right
		End_Scarf_Hangs_Right =
		    does_scarf_end_hang(Right_Rect, Max_Hanging_R_point);
		if (End_Scarf_Hangs_Right)
			Dist_HRS = Max_Hanging_R_point.V - Right_Rect.y;
		else
			Dist_HRS = -1;

		// Filling return vars
		output[0] = 0;
		output[1] = Scarf_around_Neck;
		output[2] = Dist_MS;
		output[3] = End_Scarf_Hangs_Left;
		output[4] = Dist_HLS;
		output[5] = End_Scarf_Hangs_Right;
		output[6] = Dist_HRS;

	}
	else if (!There_Is_Marker and ! There_Is_Scarf)
		output[0] = -2;
	else if (!There_Is_Marker)
		output[0] = -3;
	else if (!There_Is_Scarf)
		output[0] = -4;

	//New image processed
	Img_Refreshed = true;

}

void CheckScarf::draw_info()
{

	if (!Img_Refreshed) {
		std::cout << "Before Draw refresh Image!! check_scarf()" <<
		    std::endl;
		return;
	}

	/// Little bit of drawing to have some feedback

	if (There_Is_Marker and There_Is_Scarf) {

		// Area to check if Scarf is around the neck
		cv::rectangle(Img_rgb, Neck_Rect, 0, 2, 8, 0);
		// Scarf around or not the neck
		std::stringstream ss;
		if (Scarf_around_Neck)
			ss << "Scarf is around the Neck";
		else
			ss << "Scarf is NOT around the Neck";
		cv::Size textsize =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 0.75, 2, 0);
		cv::Point org((Img_rgb.cols - textsize.width),
			      (Img_rgb.rows - textsize.height));
		putText(Img_rgb, ss.str(), org, cv::FONT_HERSHEY_SIMPLEX, 0.75,
			0, 2, 8, false);

		// Distance Mark to Scarf
		cv::line(Img_rgb,
			 cv::Point(Marker_Centroid.U, Marker_Centroid.V),
			 cv::Point(Marker_Centroid.U,
				   Marker_Centroid.V + Dist_MS), 0, 2, 8, 0);
		ss.str("");
		ss << Dist_MS << "px";
		putText(Img_rgb, ss.str(),
			cv::Point(Marker_Centroid.U + 5,
				  Marker_Centroid.V + Dist_MS / 2),
			cv::FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);

		// Area to check if ends of scarf hang left (image)
		cv::rectangle(Img_rgb, Left_Rect, 0, 2, 8, 0);
		// Scarf is hanging or not on the left side 
		ss.str("");
		if (End_Scarf_Hangs_Left)
			ss << "End left Scarf is hanging";
		else
			ss << "End left Scarf is NOT hanging";
		cv::Size textsizeL =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 0.75, 2, 0);
		cv::Point orgL((Img_rgb.cols - textsizeL.width),
			       (Img_rgb.rows - textsizeL.height - 25));
		putText(Img_rgb, ss.str(), orgL, cv::FONT_HERSHEY_SIMPLEX, 0.75,
			0, 2, 8, false);

		// Distance hanging left side
		cv::line(Img_rgb,
			 cv::Point(Max_Hanging_L_point.U,
				   Max_Hanging_L_point.V - Dist_HLS),
			 cv::Point(Max_Hanging_L_point.U,
				   Max_Hanging_L_point.V), 0, 2, 8, 0);
		ss.str("");
		ss << Dist_HLS << "px";
		putText(Img_rgb, ss.str(),
			cv::Point(Max_Hanging_L_point.U + 5,
				  Max_Hanging_L_point.V - Dist_HLS / 2),
			cv::FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);

		// Area to check if ends of scarf hang right (image)
		cv::rectangle(Img_rgb, Right_Rect, 0, 2, 8, 0);
		// Scarf is hanging or not on the right side 
		ss.str("");
		if (End_Scarf_Hangs_Right)
			ss << "End right Scarf is hanging";
		else
			ss << "End right Scarf is NOT hanging";
		cv::Size textsizeR =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 0.75, 2, 0);
		cv::Point orgR((Img_rgb.cols - textsizeR.width),
			       (Img_rgb.rows - textsizeR.height - 50));
		putText(Img_rgb, ss.str(), orgR, cv::FONT_HERSHEY_SIMPLEX, 0.75,
			0, 2, 8, false);

		// Distance hanging right side
		cv::line(Img_rgb,
			 cv::Point(Max_Hanging_R_point.U,
				   Max_Hanging_R_point.V - Dist_HRS),
			 cv::Point(Max_Hanging_R_point.U,
				   Max_Hanging_R_point.V), 0, 2, 8, 0);
		ss.str("");
		ss << Dist_HRS << "px";
		putText(Img_rgb, ss.str(),
			cv::Point(Max_Hanging_R_point.U + 5,
				  Max_Hanging_R_point.V - Dist_HRS / 2),
			cv::FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);

	}
	else if (!There_Is_Marker and ! There_Is_Scarf) {
		std::stringstream ss;
		ss << "Neither Marker nor Scarf are found";
		cv::Size textsize =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 1, 2, 0);
		cv::Point org((Img_rgb.cols - textsize.width) / 2,
			      (Img_rgb.rows - textsize.height) / 2);
		putText(Img_rgb, ss.str(), org, cv::FONT_HERSHEY_SIMPLEX, 1, 0,
			2, 8, false);
	}
	else if (!There_Is_Marker) {
		std::stringstream ss;
		ss << "Marker not found";
		cv::Size textsize =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 1, 2, 0);
		cv::Point org((Img_rgb.cols - textsize.width) / 2,
			      (Img_rgb.rows - textsize.height) / 2);
		putText(Img_rgb, ss.str(), org, cv::FONT_HERSHEY_SIMPLEX, 1, 0,
			2, 8, false);
	}
	else if (!There_Is_Scarf) {
		std::stringstream ss;
		ss << "Scarf not found";
		cv::Size textsize =
		    getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 1, 2, 0);
		cv::Point org((Img_rgb.cols - textsize.width) / 2,
			      (Img_rgb.rows - textsize.height) / 2);
		putText(Img_rgb, ss.str(), org, cv::FONT_HERSHEY_SIMPLEX, 1, 0,
			2, 8, false);
	}

	// Show image thresholded 
	cv::imshow("Thresholded Image Mark", Img_thres_Marker);
	cv::imshow("Thresholded Image Scarf", Img_thres_Scarf);

	// Show Original image with drawing
	cv::imshow("Original", Img_rgb);

	// Once is showed, capture a new one
	Img_Refreshed = false;

}

int CheckScarf::wait_any_key()
{
	std::cout << "Press any key to exit" << std::endl;
	cv::waitKey(0);
	return 0;
}

int CheckScarf::wait(int ms)
{
	cv::waitKey(ms);
	return 0;
}
