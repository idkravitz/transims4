//*********************************************************
//	ArcTraveler.cpp - ArcView Activity/Trip/Traveler Files
//*********************************************************

#include "ArcTraveler.hpp"

char * ArcTraveler::TRAVELER_FILE				= "TRAVELER_FILE";
char * ArcTraveler::TRAVELER_FORMAT				= "TRAVELER_FORMAT";
char * ArcTraveler::ARCVIEW_TRAVELER_FILE		= "ARCVIEW_TRAVELER_FILE";
char * ArcTraveler::LANE_WIDTH					= "LANE_WIDTH";
char * ArcTraveler::CENTER_ONEWAY_LINKS			= "CENTER_ONEWAY_LINKS";
char * ArcTraveler::DRAW_NETWORK_LANES			= "DRAW_NETWORK_LANES";
char * ArcTraveler::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ArcTraveler::SELECT_TRAVELERS			= "SELECT_TRAVELERS";
char * ArcTraveler::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * ArcTraveler::SELECT_SUBAREA_POLYGON		= "SELECT_SUBAREA_POLYGON";

//---------------------------------------------------------
//	ArcTraveler constructor
//---------------------------------------------------------

ArcTraveler::ArcTraveler (void) : Demand_Service ()
{
	Program ("ArcTraveler");
	Version ("4.0.2");
	Title ("ArcView Traveler Files");

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, SHAPE, ACTIVITY_LOCATION, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		TRAVELER_FILE,
		TRAVELER_FORMAT,
		ARCVIEW_TRAVELER_FILE,
		LANE_WIDTH,
		CENTER_ONEWAY_LINKS,
		DRAW_NETWORK_LANES,
		TIME_OF_DAY_FORMAT,
		SELECT_TRAVELERS,
		SELECT_TIME_PERIODS,
		SELECT_SUBAREA_POLYGON,
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);

	projection.Add_Keys ();

	lane_width = 1.0;
	traveler_flag = trip_flag = activity_flag = false;
	lanes_flag = center_flag = shape_flag = subarea_flag = time_flag = false;
}

//---------------------------------------------------------
//	ArcTraveler destructor
//---------------------------------------------------------

ArcTraveler::~ArcTraveler (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcTraveler *exe = new ArcTraveler ();

	return (exe->Start_Execution (commands, control));
}
