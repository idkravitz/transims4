//*********************************************************
//	ArcProblem.cpp - ArcView Problem Files
//*********************************************************

#include "ArcProblem.hpp"

char * ArcProblem::PROBLEM_FILE					= "PROBLEM_FILE";
char * ArcProblem::PROBLEM_FORMAT				= "PROBLEM_FORMAT";
char * ArcProblem::ARCVIEW_PROBLEM_FILE			= "ARCVIEW_PROBLEM_FILE";
char * ArcProblem::LANE_WIDTH					= "LANE_WIDTH";
char * ArcProblem::CENTER_ONEWAY_LINKS			= "CENTER_ONEWAY_LINKS";
char * ArcProblem::DRAW_VEHICLE_SHAPES			= "DRAW_VEHICLE_SHAPES";
char * ArcProblem::DRAW_TRIP_OD_LINKS			= "DRAW_TRIP_OD_LINKS";
char * ArcProblem::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ArcProblem::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * ArcProblem::SELECT_TIME_INCREMENT		= "SELECT_TIME_INCREMENT";
char * ArcProblem::SELECT_LINKS					= "SELECT_LINKS";
char * ArcProblem::SELECT_PROBLEM_TYPES			= "SELECT_PROBLEM_TYPES";
char * ArcProblem::SELECT_SUBAREA_POLYGON		= "SELECT_SUBAREA_POLYGON";

//---------------------------------------------------------
//	ArcProblem constructor
//---------------------------------------------------------

ArcProblem::ArcProblem (void) : Demand_Service ()
{
	Program ("ArcProblem");
	Version ("4.0.3");
	Title ("ArcView Problem Files");

	Network_File required_network [] = {
		NODE, LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, ACTIVITY_LOCATION, END_NETWORK
	};

	char *keys [] = {
		PROBLEM_FILE,
		PROBLEM_FORMAT,
		ARCVIEW_PROBLEM_FILE,
		LANE_WIDTH,
		CENTER_ONEWAY_LINKS,
		DRAW_VEHICLE_SHAPES,
		DRAW_TRIP_OD_LINKS,
		TIME_OF_DAY_FORMAT,
		SELECT_TIME_PERIODS,
		SELECT_TIME_INCREMENT,
		SELECT_LINKS,
		SELECT_PROBLEM_TYPES,
		SELECT_SUBAREA_POLYGON,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);

	projection.Add_Keys ();

	lane_width = 1.0;
	center_flag = shape_flag = subarea_flag = type_flag = time_flag = location_flag = od_flag = false;
}

//---------------------------------------------------------
//	ArcProblem destructor
//---------------------------------------------------------

ArcProblem::~ArcProblem (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcProblem *exe = new ArcProblem ();

	return (exe->Start_Execution (commands, control));
}
