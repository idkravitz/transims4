//*********************************************************
//	ModeChoice.cpp - apply a mode choice model
//*********************************************************

#include "ModeChoice.hpp"

char * ModeChoice::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * ModeChoice::MODEL_PROCESSING_METHOD		= "MODEL_PROCESSING_METHOD";
char * ModeChoice::TOUR_PURPOSE_HIERARCHY		= "TOUR_PURPOSE_HIERARCHY";
char * ModeChoice::TOUR_MODE_MODEL_x			= "TOUR_MODE_MODEL_*";
char * ModeChoice::TOUR_PURPOSE_RANGE_x			= "TOUR_PURPOSE_RANGE_*";
char * ModeChoice::TRIP_MODE_MODEL_x			= "TRIP_MODE_MODEL_*";
char * ModeChoice::TRIP_PURPOSE_RANGE_x			= "TRIP_PURPOSE_RANGE_*";
char * ModeChoice::MODE_DATA_MODE_x				= "MODE_DATA_MODE_*";
char * ModeChoice::MODE_DATA_LABEL_x			= "MODE_DATA_LABEL_*";
char * ModeChoice::MODE_DATA_FIELD_x			= "MODE_DATA_FIELD_*";
char * ModeChoice::ZONE_SKIM_FILE_x				= "ZONE_SKIM_FILE_*";
char * ModeChoice::ZONE_SKIM_FORMAT_x			= "ZONE_SKIM_FORMAT_*";
char * ModeChoice::TIME_PERIOD_EQUIVALENCE_x	= "TIME_PERIOD_EQUIVALENCE_*";
char * ModeChoice::SKIM_MEMORY_FACTOR_x			= "SKIM_MEMORY_FACTOR_*";
char * ModeChoice::DISTANCE_CALCULATION			= "DISTANCE_CALCULATION";
char * ModeChoice::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * ModeChoice::MODE_SHARE_DETAILS			= "MODE_SHARE_DETAILS";
char * ModeChoice::MODE_SHARE_FORMAT			= "MODE_SHARE_FORMAT";
char * ModeChoice::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";

//---------------------------------------------------------
//	ModeChoice constructor
//---------------------------------------------------------

ModeChoice::ModeChoice (void) : Demand_Service ()
{
	Program ("ModeChoice");
	Version ("4.0.3");
	Title ("Apply a Mode Choice Model");
	
	Network_File required_network [] = {
		ACTIVITY_LOCATION, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, END_NETWORK
	};

	Demand_File required_demand [] = {
		HOUSEHOLD, END_DEMAND
	};
	Demand_File optional_demand [] = {
		POPULATION, ACTIVITY, TRIP, NEW_ACTIVITY, NEW_TRIP, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		MODEL_PROCESSING_METHOD,
		TOUR_PURPOSE_HIERARCHY,
		TOUR_MODE_MODEL_x,
		TOUR_PURPOSE_RANGE_x,
		TRIP_MODE_MODEL_x,
		TRIP_PURPOSE_RANGE_x,
		MODE_DATA_MODE_x,
		MODE_DATA_LABEL_x,
		MODE_DATA_FIELD_x,
		ZONE_SKIM_FILE_x,
		ZONE_SKIM_FORMAT_x,
		TIME_PERIOD_EQUIVALENCE_x,
		SKIM_MEMORY_FACTOR_x,
		DISTANCE_CALCULATION,
		RANDOM_NUMBER_SEED,
		MODE_SHARE_DETAILS,
		MODE_SHARE_FORMAT,
		TIME_OF_DAY_FORMAT,
		NULL
	};
	char *reports [] = {
		"TRIP_MODE_SCRIPT",
		"TRIP_MODE_STACK",
		"TOUR_MODE_SCRIPT",
		"TOUR_MODE_STACK",
		"TIME_PERIOD_EQUIVALENCE",
		"MODE_SHARE_DETAILS",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	activity_flag = trip_flag = new_act_flag = new_trip_flag = mode_flag = false;
	hhlist_flag = person_flag = skim_flag = zone_flag = share_flag = detail_flag = false;

	tot_act_in = tot_act_out = tot_act_mode = 0;
	tot_trip_in = tot_trip_out = tot_trip_mode = 0;

	trip_number = trip_mode = trip_purpose = trip_start = trip_end = trip_duration = 0;
	trip_distance = trip_ttime = tot_intra = 0;
	
	tour_first = tour_arrive = tour_return = tour_last = 0;
	tour_mode = tour_purpose = tour_start = tour_end = tour_duration = 0;
	tour_stops1 = tour_stops2 = tour_subtours = 0,
	tour_distance1 = tour_distance2 = tour_ttime1 = tour_ttime2 = 0;

	method = 0;

	memset (purpose_order, '\0', sizeof (purpose_order));
}

//---------------------------------------------------------
//	ModeChoice destructor
//---------------------------------------------------------

ModeChoice::~ModeChoice (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ModeChoice *exe = new ModeChoice ();

	return (exe->Start_Execution (commands, control));
}
