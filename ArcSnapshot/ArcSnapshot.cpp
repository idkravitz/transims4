//*********************************************************
//	ArcSnapshot.cpp - ArcView Snapshot Files
//*********************************************************

#include "ArcSnapshot.hpp"

char * ArcSnapshot::SNAPSHOT_FILE				= "SNAPSHOT_FILE";
char * ArcSnapshot::SNAPSHOT_FORMAT				= "SNAPSHOT_FORMAT";
char * ArcSnapshot::OCCUPANCY_FILE				= "OCCUPANCY_FILE";
char * ArcSnapshot::OCCUPANCY_FORMAT			= "OCCUPANCY_FORMAT";
char * ArcSnapshot::ARCVIEW_SNAPSHOT_FILE		= "ARCVIEW_SNAPSHOT_FILE";
char * ArcSnapshot::CELL_SIZE					= "CELL_SIZE";
char * ArcSnapshot::LANE_WIDTH					= "LANE_WIDTH";
char * ArcSnapshot::CENTER_ONEWAY_LINKS			= "CENTER_ONEWAY_LINKS";
char * ArcSnapshot::DRAW_VEHICLE_SHAPES			= "DRAW_VEHICLE_SHAPES";
char * ArcSnapshot::ADD_PASSENGER_CIRCLE_SIZE	= "ADD_PASSENGER_CIRCLE_SIZE";
char * ArcSnapshot::ADD_PASSENGER_SQUARE_SIZE	= "ADD_PASSENGER_SQUARE_SIZE";
char * ArcSnapshot::TIME_STEPS_PER_SECOND		= "TIME_STEPS_PER_SECOND";
char * ArcSnapshot::PAD_FILE_TIME_LABEL			= "PAD_FILE_TIME_LABEL";
char * ArcSnapshot::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ArcSnapshot::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * ArcSnapshot::SELECT_TIME_INCREMENT		= "SELECT_TIME_INCREMENT";
char * ArcSnapshot::SELECT_SUBAREA_POLYGON		= "SELECT_SUBAREA_POLYGON";
char * ArcSnapshot::SELECT_VEHICLE_TYPES		= "SELECT_VEHICLE_TYPES";
char * ArcSnapshot::TIME_PROCESSING_METHOD		= "TIME_PROCESSING_METHOD";
char * ArcSnapshot::NEW_SNAPSHOT_FILE			= "NEW_SNAPSHOT_FILE";
char * ArcSnapshot::NEW_SNAPSHOT_FORMAT			= "NEW_SNAPSHOT_FORMAT";
char * ArcSnapshot::NEW_LINK_SUMMARY_FILE		= "NEW_LINK_SUMMARY_FILE";
char * ArcSnapshot::NEW_LINK_SUMMARY_FORMAT		= "NEW_LINK_SUMMARY_FORMAT";

//---------------------------------------------------------
//	ArcSnapShot constructor
//---------------------------------------------------------

ArcSnapshot::ArcSnapshot (void) : Demand_Service ()
{
	Program ("ArcSnapshot");
	Version ("4.0.14");
	Title ("ArcView Snapshot Files");

	Network_File required_network [] = {
		NODE, LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, END_NETWORK
	};

	Demand_File required_demand [] = {
		VEHICLE_TYPE, END_DEMAND
	};

	Demand_File optional_demand [] = {
		END_DEMAND
	};

	char *keys [] = {
		SNAPSHOT_FILE,
		SNAPSHOT_FORMAT,
		OCCUPANCY_FILE,
		OCCUPANCY_FORMAT,
		ARCVIEW_SNAPSHOT_FILE,
		CELL_SIZE,
		LANE_WIDTH,
		CENTER_ONEWAY_LINKS,
		DRAW_VEHICLE_SHAPES,
		ADD_PASSENGER_CIRCLE_SIZE,
		ADD_PASSENGER_SQUARE_SIZE,
		TIME_STEPS_PER_SECOND,
		PAD_FILE_TIME_LABEL,
		TIME_OF_DAY_FORMAT,
		SELECT_TIME_PERIODS,
		SELECT_TIME_INCREMENT,
		SELECT_SUBAREA_POLYGON,
		SELECT_VEHICLE_TYPES,
		TIME_PROCESSING_METHOD,
		NEW_SNAPSHOT_FILE,
		NEW_SNAPSHOT_FORMAT,
		NEW_LINK_SUMMARY_FILE,
		NEW_LINK_SUMMARY_FORMAT,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);

	projection.Add_Keys ();

	method = AT_INCREMENT;
	max_time = MIDNIGHT;
	nsaved = 0;
	lane_width = 1.0;
	cell_size = 7.5;
	steps_per_second = 1;

	center_flag = shape_flag = subarea_flag = snapshot_flag = occupancy_flag = output_flag = project_flag = false;
	pad_flag = sum_flag = veh_type_flag = circle_flag = square_flag = false;

	link_field = dir_field = offset_field = lane_field = type_field = occ_field = start_field = end_field = 0;
	link_fld = start_fld = end_fld = num_links = num_sum = circle_field = square_field = 0;

	veh_fld = per_fld = NULL;
	vehicles = persons = NULL;

	cell_data.Initialize (sizeof (Cell_Data));
}

//---------------------------------------------------------
//	ArcSnapshot destructor
//---------------------------------------------------------

ArcSnapshot::~ArcSnapshot (void)
{
	int i;

	if (vehicles != NULL) {
		for (i=0; i < max_type; i++) {
			if (vehicles [i] != NULL) {
				delete [] vehicles [i];
				vehicles [i] = NULL;
			}
		}
		delete [] vehicles;
		vehicles = NULL;
	}
	if (persons != NULL) {
		for (i=0; i < max_type; i++) {
			if (persons [i] != NULL) {
				delete [] persons [i];
				persons [i] = NULL;
			}
		}
		delete [] persons;
		persons = NULL;
	}
	if (veh_fld != NULL) {
		delete [] veh_fld;
		veh_fld = NULL;
	}
	if (per_fld != NULL) {
		delete [] per_fld;
		per_fld = NULL;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcSnapshot *exe = new ArcSnapshot ();

	return (exe->Start_Execution (commands, control));
}
