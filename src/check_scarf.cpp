#include "check_scarf.h"

CheckScarf::CheckScarf()
{
	Marker_Centroid.U = -1;
	Marker_Centroid.V = -1;
	Dist_MS = -1;
	There_Is_Marker = false;
	There_Is_Scarf = false;
	Scarf_around_Neck = false;
	Img_Refreshed = false;
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
		std::cout << "Cannot read a frame from video stream" <<
		    std::endl;

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

void Check_Rect_Values(cv::Rect &R, int rows, int cols)
{
	if (R.x < 0) {
		R.width=R.width+R.x;
		R.x = 0;
	}
	if (R.y < 0) {
		R.height=R.height+R.y;
		R.y = 0;
	}
	if (R.x+R.width > cols)
		R.width = cols-R.x;
	if (R.y+R.height > rows)
		R.height = rows-R.y;
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

bool CheckScarf::is_scarf_around_neck()
{

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

bool CheckScarf::does_scarf_end_hang(cv::Rect const &R)
{

	int nRows = R.y + R.height;
	int channels = Img_thres_Scarf.channels();
	int nCols = (R.x + R.width) * channels;
	bool trobat = false;
	int i = R.x;

	// Trying to find a stright Vertical white line
	Centroid Max_length;
	Max_length.U=R.x;
	Max_length.V=R.y;
	while (i < nCols and ! trobat) {
		int j = R.y;
		bool black_px = false;

		while (j < nRows and ! trobat and ! black_px) {
			if (Img_thres_Scarf.at<uchar>(j,i) == 0)
				black_px = true;
			Img_thres_Scarf.at<uchar>(j,i)=128;		//To Debug
			j++;
		}

		if (j-1>Max_length.V) {
			Max_length.U=i;
			Max_length.V=j;
		}

		if (j==nRows)
			trobat = true;

		i++;
	}

	if (Max_length.V > R.y) trobat=true;

	return trobat;

}



void CheckScarf::check_scarf(std::vector < int >&output)
{
	// Colors definition 
	int const Colors_HSV_Detect[2][6] = {	//LowH HighH LowS HighS LowV HighV
		149, 179, 0, 255, 160, 255,	//Marker Fucsia 
		25, 45, 100, 255, 40, 200	//Scarf Green 
	};

	// Init output var
	output.resize(2);

	// Capturing camera image 
	capture_image();

	// Color segmentation
	image_color_segmentation(Colors_HSV_Detect[0], Img_thres_Marker);
	There_Is_Marker = (cv::countNonZero(Img_thres_Marker) > 0);
	image_color_segmentation(Colors_HSV_Detect[1], Img_thres_Scarf);
	There_Is_Scarf = (cv::countNonZero(Img_thres_Scarf) > 0);

	// Processing Image 
	if (There_Is_Marker and There_Is_Scarf) {

		// Vertical Marker-Scarf distance
		Dist_MS =
		    calc_mark_scarf_distance(Img_thres_Marker, Img_thres_Scarf,
					     Marker_Centroid.U,
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

		//Checking if Ends of scarf hang (left)
		// Init neck ROI
		Left_Rect.x = Marker_Centroid.U - 140;
		Left_Rect.y = Marker_Centroid.V + 130;
		Left_Rect.width = 100;
		Left_Rect.height = Img_rgb.rows-Left_Rect.y;
		// Check if ROI is OK to Prevent segmentation fault
		Check_Rect_Values(Left_Rect, Img_rgb.rows, Img_rgb.cols);
		// Checking in ROI if Scarf is hanging left 
		End_Scarf_Hangs_Left=does_scarf_end_hang(Left_Rect);

		//Checking if Ends of scarf hang (right)
		// Init neck ROI
		Right_Rect.x = Marker_Centroid.U + 40;
		Right_Rect.y = Marker_Centroid.V + 130;
		Right_Rect.width = 100;
		Right_Rect.height = Img_rgb.rows-Left_Rect.y;
		// Check if ROI is OK to Prevent segmentation fault
		Check_Rect_Values(Right_Rect, Img_rgb.rows, Img_rgb.cols);
		// Checking in ROI if Scarf is hanging right
		End_Scarf_Hangs_Right=does_scarf_end_hang(Right_Rect);


		// Filling return vars
		output[0] = Dist_MS;
		output[1] = Scarf_around_Neck;

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
		// Distance Mark to Scarf
		cv::line(Img_rgb,
			 cv::Point(Marker_Centroid.U, Marker_Centroid.V),
			 cv::Point(Marker_Centroid.U,
				   Marker_Centroid.V + Dist_MS), 0, 2, 8, 0);
		std::stringstream ss;
		ss << Dist_MS << "px";
		putText(Img_rgb, ss.str(),
			cv::Point(Marker_Centroid.U + 5,
				  Marker_Centroid.V + Dist_MS / 2),
			cv::FONT_HERSHEY_SIMPLEX, 1, 0, 2, 8, false);

		// Area to check if Scarf is around the neck
		cv::rectangle(Img_rgb, Neck_Rect, 0, 2, 8, 0);
		// Scarf around or not the neck
		ss.str("");
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
			      (Img_rgb.rows - textsizeL.height-25));
		putText(Img_rgb, ss.str(), orgL, cv::FONT_HERSHEY_SIMPLEX, 0.75,
			0, 2, 8, false);

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
			      (Img_rgb.rows - textsizeR.height-50));
		putText(Img_rgb, ss.str(), orgR, cv::FONT_HERSHEY_SIMPLEX, 0.75,
			0, 2, 8, false);


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
