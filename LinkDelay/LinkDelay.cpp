//*********************************************************
//	LinkDelay.cpp - manipulate the link delay file
//*********************************************************

#include "LinkDelay.hpp"

char * LinkDelay::PREVIOUS_LINK_DELAY_FILE		= "PREVIOUS_LINK_DELAY_FILE";
char * LinkDelay::PREVIOUS_LINK_DELAY_FORMAT	= "PREVIOUS_LINK_DELAY_FORMAT";
char * LinkDelay::PREVIOUS_WEIGHTING_FACTOR		= "PREVIOUS_WEIGHTING_FACTOR";
char * LinkDelay::PREVIOUS_LINK_FILE			= "PREVIOUS_LINK_FILE";
char * LinkDelay::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * LinkDelay::PROCESSING_METHOD				= "PROCESSING_METHOD";
char * LinkDelay::SMOOTH_GROUP_SIZE				= "SMOOTH_GROUP_SIZE";
char * LinkDelay::PERCENT_MOVED_FORWARD			= "PERCENT_MOVED_FORWARD";
char * LinkDelay::PERCENT_MOVED_BACKWARD		= "PERCENT_MOVED_BACKWARD";
char * LinkDelay::NUMBER_OF_ITERATIONS			= "NUMBER_OF_ITERATIONS";
char * LinkDelay::CIRCULAR_GROUP_FLAG			= "CIRCULAR_GROUP_FLAG";
char * LinkDelay::TIME_PERIOD_SORT				= "TIME_PERIOD_SORT";

//---------------------------------------------------------
//	LinkDelay constructor
//---------------------------------------------------------

LinkDelay::LinkDelay (void) : Demand_Service ()
{
	Program ("LinkDelay");
	Version ("4.0.11");
	Title ("Manipulate the Link Delay File");

	Network_File required_network [] = {
		LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, LANE_CONNECTIVITY, END_NETWORK
	};

	Demand_File required_demand [] = {
		NEW_LINK_DELAY, END_DEMAND
	};
	Demand_File optional_demand [] = {
		LINK_DELAY, END_DEMAND
	};
	char *keys [] = {
		PREVIOUS_LINK_DELAY_FILE,
		PREVIOUS_LINK_DELAY_FORMAT,
		PREVIOUS_WEIGHTING_FACTOR,
		PREVIOUS_LINK_FILE,
		PROCESSING_METHOD,
		SMOOTH_GROUP_SIZE,
		PERCENT_MOVED_FORWARD,
		PERCENT_MOVED_BACKWARD,
		NUMBER_OF_ITERATIONS,
		CIRCULAR_GROUP_FLAG,
		TIME_PERIOD_SORT,
		NULL
	};
	Key_List (keys);

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	previous_flag = link_flag = false;
	method = SIMPLE_AVERAGE;
	factor = 1.0;
	nerror = 0;

	niter = 3;
	naverage = 3;
	forward = 20.0;
	backward = 20.0;
	loop_flag = true;
	sort_flag = false;
}

//---------------------------------------------------------
//	LinkDelay destructor
//---------------------------------------------------------

LinkDelay::~LinkDelay (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkDelay *exe = new LinkDelay ();

	return (exe->Start_Execution (commands, control));
}
