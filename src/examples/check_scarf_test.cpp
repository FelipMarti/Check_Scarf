#include "check_scarf.h"
#include <iostream>

int main(int argc, char *argv[])
{

	std::
	    cout << "This is an example of how to check the scarf" << std::endl;
	// Create a CheckScarf class;
	CheckScarf c;

	while (true) {

		// Check and get values
		std::vector < int >v;
		c.check_scarf(v);

		if (v[0] < 0) {
			std::cout << "Something wrong happen. Error code: " <<
			    v[0] << std::endl;
		}
		else {
			std::cout << "the distance Mark-Scarf is: " << v[0] <<
			    "px" << std::endl;
			if (v[1])
				std::cout << "The Scarf is arround the neck" <<
				    std::endl;
			else
				std::cout << "The Scarf is NOT arround the neck"
				    << std::endl;

		}
		// Show images
		c.draw_info();

		// This function is necessary to display images
		c.wait(500);	// half second

	}
	//c.wait_any_key();    // any key

	return 0;

}
