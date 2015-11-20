//*********************************************************
//	LocationData.cpp - Activity Location Data Processing
//*********************************************************

#include "LocationData.hpp"

char * LocationData::COPY_EXISTING_FIELDS		= "COPY_EXISTING_FIELDS";
char * LocationData::NEW_WALK_ACCESS_FIELD		= "NEW_WALK_ACCESS_FIELD";
char * LocationData::MAX_WALK_DISTANCE			= "MAX_WALK_DISTANCE";
char * LocationData::WALK_ACCESS_TIME_RANGE		= "WALK_ACCESS_TIME_RANGE";
char * LocationData::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * LocationData::NEW_USE_FLAG_FIELD			= "NEW_USE_FLAG_FIELD";
char * LocationData::LINK_USE_FLAG_TYPES		= "LINK_USE_FLAG_TYPES";
char * LocationData::NEW_USE_FLAG_FIELD_x		= "NEW_USE_FLAG_FIELD_*";
char * LocationData::LINK_USE_FLAG_TYPES_x		= "LINK_USE_FLAG_TYPES_*";
char * LocationData::SUBZONE_ZONE_FACTOR_FILE	= "SUBZONE_ZONE_FACTOR_FILE";
char * LocationData::NEW_SUBZONE_FIELD			= "NEW_SUBZONE_FIELD";
char * LocationData::MAX_SUBZONE_DISTANCE		= "MAX_SUBZONE_DISTANCE";
char * LocationData::SUBZONE_DATA_FILE			= "SUBZONE_DATA_FILE";
char * LocationData::SUBZONE_DATA_FORMAT		= "SUBZONE_DATA_FORMAT";
char * LocationData::SUBZONE_DATA_FIELD			= "SUBZONE_DATA_FIELD";
char * LocationData::SUBZONE_ZONE_FIELD			= "SUBZONE_ZONE_FIELD";
char * LocationData::NEW_SUBZONE_FIELD_x		= "NEW_SUBZONE_FIELD_*";
char * LocationData::MAX_SUBZONE_DISTANCE_x		= "MAX_SUBZONE_DISTANCE_*";
char * LocationData::SUBZONE_DATA_FILE_x		= "SUBZONE_DATA_FILE_*";
char * LocationData::SUBZONE_DATA_FORMAT_x		= "SUBZONE_DATA_FORMAT_*";
char * LocationData::SUBZONE_DATA_FIELD_x		= "SUBZONE_DATA_FIELD_*";
char * LocationData::SUBZONE_ZONE_FIELD_x		= "SUBZONE_ZONE_FIELD_*";
char * LocationData::NEW_LOCATION_FIELD			= "NEW_LOCATION_FIELD";
char * LocationData::NEW_LOCATION_FIELD_x		= "NEW_LOCATION_FIELD_*";
char * LocationData::CONVERSION_SCRIPT			= "CONVERSION_SCRIPT";
char * LocationData::DATA_FILE					= "DATA_FILE";
char * LocationData::DATA_FORMAT				= "DATA_FORMAT";
char * LocationData::DATA_JOIN_FIELD			= "DATA_JOIN_FIELD";
char * LocationData::LOCATION_JOIN_FIELD		= "LOCATION_JOIN_FIELD";
char * LocationData::DATA_FILE_x				= "DATA_FILE_*";
char * LocationData::DATA_FORMAT_x				= "DATA_FORMAT_*";
char * LocationData::DATA_JOIN_FIELD_x			= "DATA_JOIN_FIELD_*";
char * LocationData::LOCATION_JOIN_FIELD_x		= "LOCATION_JOIN_FIELD_*";
char * LocationData::ZONE_BOUNDARY_POLYGON		= "ZONE_BOUNDARY_POLYGON";
char * LocationData::ZONE_FIELD_NAME			= "ZONE_FIELD_NAME";
char * LocationData::ZONE_UPDATE_RANGE			= "ZONE_UPDATE_RANGE";
char * LocationData::BOUNDARY_POLYGON			= "BOUNDARY_POLYGON";
char * LocationData::BOUNDARY_POLYGON_x			= "BOUNDARY_POLYGON_*";

//---------------------------------------------------------
//	LocationData constructor
//---------------------------------------------------------

LocationData::LocationData (void) : Network_Service ()
{
	Program ("LocationData");
	Version ("4.0.12");
	Title ("Activity Location Data Processor");

	Network_File required_network [] = {
		ACTIVITY_LOCATION, NEW_ACTIVITY_LOCATION, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NEW_DIRECTORY, 
		LINK, NODE, ZONE, PROCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, END_NETWORK
	};

	char *keys [] = {
		COPY_EXISTING_FIELDS,
		NEW_WALK_ACCESS_FIELD,
		MAX_WALK_DISTANCE,
		WALK_ACCESS_TIME_RANGE,
		TIME_OF_DAY_FORMAT,
		NEW_USE_FLAG_FIELD,
		LINK_USE_FLAG_TYPES,
		NEW_USE_FLAG_FIELD_x,
		LINK_USE_FLAG_TYPES_x,
		SUBZONE_ZONE_FACTOR_FILE,
		NEW_SUBZONE_FIELD,
		MAX_SUBZONE_DISTANCE,
		SUBZONE_DATA_FILE,
		SUBZONE_DATA_FORMAT,
		SUBZONE_DATA_FIELD,
		SUBZONE_ZONE_FIELD,
		NEW_SUBZONE_FIELD_x,
		MAX_SUBZONE_DISTANCE_x,
		SUBZONE_DATA_FILE_x,
		SUBZONE_DATA_FORMAT_x,
		SUBZONE_DATA_FIELD_x,
		SUBZONE_ZONE_FIELD_x,
		NEW_LOCATION_FIELD,
		NEW_LOCATION_FIELD_x,
		CONVERSION_SCRIPT,
		DATA_FILE,
		DATA_FORMAT,
		DATA_JOIN_FIELD,
		LOCATION_JOIN_FIELD,
		DATA_FILE_x,
		DATA_FORMAT_x,
		DATA_JOIN_FIELD_x,
		LOCATION_JOIN_FIELD_x,
		ZONE_BOUNDARY_POLYGON,
		ZONE_FIELD_NAME,
		ZONE_UPDATE_RANGE,
		BOUNDARY_POLYGON,
		BOUNDARY_POLYGON_x,
		NULL
	};

	char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"CHECK_ZONE_COVERAGE",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);

	projection.Add_Keys ();

	copy_flag = false;
	walk_access_flag = walk_time_flag = false;
	walk_access_field = 0;
	walk_distance = 1000;

	data_files = NULL;
	num_data_files = 0;
	data_flag = false;

	data_field = NULL;
	location_field = NULL;
	data_db = NULL;
	data_rec = NULL;

	zone_file_flag = subzone_map_flag = false;
	num_subzone = 0;
	subzone = NULL;
	script_flag = false;

	use_flag = false;

	zone_flag = coverage_flag = false;
	zone_field = 0;
	zone_boundary.Data_Size (sizeof (Boundary));
	zone_map.Data_Size (sizeof (Zone_Map));
	subzone_weight.Initialize (sizeof (Weight), 2);
	subzone_map.Initialize (sizeof (Subzone_Map), 2);
	loc_weight.Initialize (sizeof (Loc_Weight));

	num_polygons = 0;
	polygon_flag = false;
	polygon_files = NULL;
	polygon_db = NULL;
	polygons = NULL;
}

//---------------------------------------------------------
//	LocationData destructor
//---------------------------------------------------------

LocationData::~LocationData (void)
{
	int i;

	if (data_files != NULL) {
		for (i=0; i < num_data_files; i++) {
			if (data_files [i] != NULL) {
				data_files [i]->Close ();
				delete data_files [i];
			}
		}
		delete [] data_files;
	}
	if (data_db != NULL) {
		for (i=0; i < num_data_files; i++) {
			if (data_db [i] != NULL) {
				delete data_db [i];
			}
		}
		delete [] data_db;
	}
	if (data_field != NULL) {
		delete [] data_field;
	}
	if (location_field != NULL) {
		delete [] location_field;
	}
	if (data_rec != NULL) {
		delete [] data_rec;
	}
	if (subzone != NULL) {
		for (i=0; i < num_subzone; i++) {
			if (subzone [i] != NULL) {
				delete subzone [i];
			}
		}
		delete [] subzone;
	}
	if (zone_flag) {
		Boundary *ptr;

		for (ptr = (Boundary *) zone_boundary.First (); ptr; ptr = (Boundary *) zone_boundary.Next ()) {
			delete ptr->points;
		}
	}
	if (polygon_flag) {
		Boundary *ptr;
		Index_Array *poly_ptr;

		for (i=0; i < num_polygons; i++) {
			poly_ptr = polygons [i];
			if (poly_ptr != NULL) {
				for (ptr = (Boundary *) poly_ptr->First (); ptr; ptr = (Boundary *) poly_ptr->Next ()) {
					delete ptr->points;
				}
				delete poly_ptr;
			}
			if (polygon_db [i] != NULL) {
				delete polygon_db [i];
			}
			delete [] polygon_db;

			if (polygon_files [i] != NULL) {
				delete polygon_files [i];
			}
			delete [] polygon_files;
		}
		delete [] polygons;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LocationData *exe = new LocationData ();

	return (exe->Start_Execution (commands, control));
}
