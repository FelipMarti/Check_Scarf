#include "check_scarf.h"
#include <iostream>

int main(int argc, char *argv[])
{

	// Create a CheckScarf class;
	CheckScarf c;

	while (true) {

		// Set Marker Color
						// LowH HighH LowS HighS LowV HighV
		int HSV_Marker[6] = {149, 179, 0, 255, 160, 255};
		c.set_HSV_Color(true,HSV_Marker);

		// Set Scarf Color
						// LowH HighH LowS HighS LowV HighV
		int HSV_Scarf[6] = {25, 45, 125, 255, 40, 200};
		c.set_HSV_Color(false,HSV_Scarf);

		// Output var is a vector
		std::vector < int >v;
		// Check scarf 
		c.check_scarf(v);

		// Understanding the output
		if (v[0] < 0) {
			std::cout << "Error code: " << v[0] << std::endl;
			if (v[0] == -2)
				std::cout <<
				    "Neither Marker nor Scarf are found" <<
				    std::endl;
			if (v[0] == -3)
				std::cout << "Marker not found" << std::endl;
			if (v[0] == -4)
				std::cout << "Scarf not found" << std::endl;

			std::cout << std::endl;

		}
		else {
			if (v[1]) {
				std::cout << "The Scarf is around the neck at: "
				    << v[2] << "px" << std::endl;
			}
			else {
				std::cout << "The Scarf is NOT around the neck"
				    << std::endl;
			}

			if (v[3]) {
				std::cout <<
				    "The Scarf end is hanging on left side at: "
				    << v[4] << "px" << std::endl;
			}
			else {
				std::cout <<
				    "The Scarf is NOT hanging on left side" <<
				    std::endl;
			}

			if (v[5]) {
				std::cout <<
				    "The Scarf end is hanging on right side at: "
				    << v[6] << "px" << std::endl;
			}
			else {
				std::cout <<
				    "The Scarf is NOT hanging on right side" <<
				    std::endl;
			}

			std::cout << std::endl;

		}

		// Show images
		c.draw_info();

		// This function is necessary to display images
		c.wait(500);	// half second

	}
	//c.wait_any_key();    // any key

	return 0;

}
