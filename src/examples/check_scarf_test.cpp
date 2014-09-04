#include "check_scarf.h"
#include <iostream>

int main(int argc, char *argv[])
{

	std::cout<<"This is an example of how to check the scarf"<<std::endl;
	// Create a CheckScarf class;
	CheckScarf c;

	// Check and get values
	int distance = c.check_scarf();
	std::cout<<"the value obtained is: "<<distance<<"px"<<std::endl;

	// Show images
	c.draw_info();

	// This function is necessary to display images
	c.wait();				// some seconds
//	c.wait_any_key();		// any key

	return 0;

}
