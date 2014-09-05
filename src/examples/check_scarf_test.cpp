#include "check_scarf.h"
#include <iostream>

int main(int argc, char *argv[])
{

	std::cout << "This is an example of how to check the scarf" << std::
	    endl;
	// Create a CheckScarf class;
	CheckScarf c;

	while (true) {

		// Check and get values
		int distance = c.check_scarf();
		if (distance < 0)
			std::
			    cout << "Something wrong happen. Error code: " <<
			    distance << std::endl;
		else
			std::
			    cout << "the value obtained is: " << distance <<
			    "px" << std::endl;

		// Show images
		c.draw_info();

		// This function is necessary to display images
		c.wait(500);	// half second

	}
	//      c.wait_any_key();    // any key

	return 0;

}
