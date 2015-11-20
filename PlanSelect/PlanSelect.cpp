//*********************************************************
//	PlanSelect.cpp - Select Households for Re-Routing
//*********************************************************

#include "PlanSelect.hpp"

char * PlanSelect::PLAN_FILE					= "PLAN_FILE";
char * PlanSelect::PLAN_FORMAT					= "PLAN_FORMAT";
char * PlanSelect::NODE_LIST_PATHS				= "NODE_LIST_PATHS";
char * PlanSelect::POPULATION_FILE				= "POPULATION_FILE";
char * PlanSelect::NEW_HOUSEHOLD_LIST			= "NEW_HOUSEHOLD_LIST";
char * PlanSelect::NEW_HOUSEHOLD_RECORD_FILE	= "NEW_HOUSEHOLD_RECORD_FILE";
char * PlanSelect::NEW_HOUSEHOLD_RECORD_FORMAT	= "NEW_HOUSEHOLD_RECORD_FORMAT";
char * PlanSelect::SELECT_TRAVELERS				= "SELECT_TRAVELERS";
char * PlanSelect::SELECT_NODES_x				= "SELECT_NODES_*";
char * PlanSelect::SELECT_LINKS_x				= "SELECT_LINKS_*";
char * PlanSelect::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * PlanSelect::SELECT_TRAVEL_MODES			= "SELECT_TRAVEL_MODES";
char * PlanSelect::SELECT_FACILITY_TYPES		= "SELECT_FACILITY_TYPES";
char * PlanSelect::SELECT_LOCATIONS				= "SELECT_LOCATIONS";
char * PlanSelect::SELECT_PARKING_LOTS			= "SELECT_PARKING_LOTS";
char * PlanSelect::SELECT_TRANSIT_STOPS			= "SELECT_TRANSIT_STOPS";
char * PlanSelect::SELECT_TRANSIT_ROUTES		= "SELECT_TRANSIT_ROUTES";
char * PlanSelect::SELECT_VC_RATIOS				= "SELECT_VC_RATIOS";
char * PlanSelect::SELECT_TIME_RATIOS			= "SELECT_TIME_RATIOS";
char * PlanSelect::SELECT_COORDINATES			= "SELECT_COORDINATES";
char * PlanSelect::SELECT_COORDINATES_x			= "SELECT_COORDINATES_*";
char * PlanSelect::SELECT_OD_COORDINATES		= "SELECT_OD_COORDINATES";
char * PlanSelect::SELECT_OD_COORDINATES_x		= "SELECT_OD_COORDINATES_*";
char * PlanSelect::EXCLUDE_OD_COORDINATES		= "EXCLUDE_OD_COORDINATES";
char * PlanSelect::EXCLUDE_OD_COORDINATES_x		= "EXCLUDE_OD_COORDINATES_*";
char * PlanSelect::SELECT_SUBAREA_POLYGON		= "SELECT_SUBAREA_POLYGON";
char * PlanSelect::SELECT_VEHICLE_TYPES			= "SELECT_VEHICLE_TYPES";
char * PlanSelect::PERCENT_TIME_DIFFERENCE		= "PERCENT_TIME_DIFFERENCE";
char * PlanSelect::MINIMUM_TIME_DIFFERENCE		= "MINIMUM_TIME_DIFFERENCE";
char * PlanSelect::MAXIMUM_TIME_DIFFERENCE		= "MAXIMUM_TIME_DIFFERENCE";
char * PlanSelect::SELECTION_PERCENTAGE			= "SELECTION_PERCENTAGE";
char * PlanSelect::MAXIMUM_PERCENT_SELECTED		= "MAXIMUM_PERCENT_SELECTED";
char * PlanSelect::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";

//---------------------------------------------------------
//	PlanSelect constructor
//---------------------------------------------------------

PlanSelect::PlanSelect (void) : Demand_Service ()
{
	Program ("PlanSelect");
	Version ("4.0.32");
	Title ("Select Households for Re-Routing");

	Network_File required_network [] = {
		END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, LANE_USE, PARKING, LANE_CONNECTIVITY, 
		TRANSIT_STOP, TRANSIT_DRIVER, END_NETWORK
	};

	Demand_File required_demand [] = {
		END_DEMAND
	};

	Demand_File optional_demand [] = {
		LINK_DELAY, VEHICLE, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		POPULATION_FILE,
		NEW_HOUSEHOLD_LIST,
		NEW_HOUSEHOLD_RECORD_FILE,
		NEW_HOUSEHOLD_RECORD_FORMAT,
		SELECT_TRAVELERS,
		SELECT_NODES_x,
		SELECT_LINKS_x,
		SELECT_TIME_PERIODS,
		SELECT_TRAVEL_MODES,
		SELECT_FACILITY_TYPES,
		SELECT_LOCATIONS,
		SELECT_TRANSIT_STOPS,
		SELECT_TRANSIT_ROUTES,
		SELECT_PARKING_LOTS,
		SELECT_VC_RATIOS,
		SELECT_TIME_RATIOS,
		SELECT_COORDINATES,
		SELECT_COORDINATES_x,
		SELECT_OD_COORDINATES,
		SELECT_OD_COORDINATES_x,
		EXCLUDE_OD_COORDINATES,
		EXCLUDE_OD_COORDINATES_x,
		SELECT_SUBAREA_POLYGON,
		SELECT_VEHICLE_TYPES,
		PERCENT_TIME_DIFFERENCE,
		MINIMUM_TIME_DIFFERENCE,
		MAXIMUM_TIME_DIFFERENCE,
		SELECTION_PERCENTAGE,
		MAXIMUM_PERCENT_SELECTED,
		RANDOM_NUMBER_SEED,
		NULL
	};
	char *reports [] = {
		"SUBAREA_INTERNAL_EXTERNAL",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	AB_Key_Flag  (true);

	type_flag = true;
	delay_flag = pop_flag = vc_flag = diff_flag = select_flag = vehicle_flag = ratio_flag = false;
	parking_flag = turn_flag = coord_flag = in_flag = out_flag = subarea_flag = hhrec_flag = false;
	factype_flag = false;
	num_hhold = 0;
	num_ii = num_ie = num_ei = num_ee = 0;

	max_time = 60;
	min_time = 1;

	vc_ratio = 1.0;
	time_ratio = 2.0;
	percent_diff = 10.0;
	percent = 100.0;
	max_percent = 100.0;

	coord_box.Initialize (sizeof (Box), 5);
	od_box.Initialize (sizeof (Box), 5);
	exclude_box.Initialize (sizeof (Box), 5);
	
	nodes.Initialize (sizeof (Data_Range));
	links.Initialize (sizeof (Data_Range));
}

//---------------------------------------------------------
//	PlanSelect destructor
//---------------------------------------------------------

PlanSelect::~PlanSelect (void)
{
	Data_Range *range;

	for (range = (Data_Range *) nodes.First (); range != NULL; range = (Data_Range *) nodes.Next ()) {
		range->Clear ();
	}
	for (range = (Data_Range *) links.First (); range != NULL; range = (Data_Range *) links.Next ()) {
		range->Clear ();
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanSelect *exe = new PlanSelect ();

	return (exe->Start_Execution (commands, control));
}


