//*********************************************************
//	ZoneData.cpp - Zone Data Processing
//*********************************************************

#include "ZoneData.hpp"

char * ZoneData::COPY_EXISTING_FIELDS		= "COPY_EXISTING_FIELDS";
char * ZoneData::NEW_ZONE_FIELD				= "NEW_ZONE_FIELD";
char * ZoneData::NEW_ZONE_FIELD_x			= "NEW_ZONE_FIELD_*";
char * ZoneData::CONVERSION_SCRIPT			= "CONVERSION_SCRIPT";
char * ZoneData::DATA_FILE					= "DATA_FILE";
char * ZoneData::DATA_FORMAT				= "DATA_FORMAT";
char * ZoneData::DATA_JOIN_FIELD			= "DATA_JOIN_FIELD";
char * ZoneData::ZONE_JOIN_FIELD			= "ZONE_JOIN_FIELD";
char * ZoneData::DATA_FILE_x				= "DATA_FILE_*";
char * ZoneData::DATA_FORMAT_x				= "DATA_FORMAT_*";
char * ZoneData::DATA_JOIN_FIELD_x			= "DATA_JOIN_FIELD_*";
char * ZoneData::ZONE_JOIN_FIELD_x			= "ZONE_JOIN_FIELD_*";
char * ZoneData::BOUNDARY_POLYGON			= "BOUNDARY_POLYGON";
char * ZoneData::BOUNDARY_POLYGON_x			= "BOUNDARY_POLYGON_*";

//---------------------------------------------------------
//	ZoneData constructor
//---------------------------------------------------------

ZoneData::ZoneData (void) : Network_Service ()
{
	Program ("ZoneData");
	Version ("4.0.1");
	Title ("Zone Data Processor");

	Network_File required_network [] = {
		ZONE, NEW_ZONE, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NEW_DIRECTORY, END_NETWORK
	};

	char *keys [] = {
		COPY_EXISTING_FIELDS,
		NEW_ZONE_FIELD,
		NEW_ZONE_FIELD_x,
		CONVERSION_SCRIPT,
		DATA_FILE,
		DATA_FORMAT,
		DATA_JOIN_FIELD,
		ZONE_JOIN_FIELD,
		DATA_FILE_x,
		DATA_FORMAT_x,
		DATA_JOIN_FIELD_x,
		ZONE_JOIN_FIELD_x,
		BOUNDARY_POLYGON,
		BOUNDARY_POLYGON_x,
		NULL
	};

	char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);

	projection.Add_Keys ();

	copy_flag = false;

	data_files = NULL;
	num_data_files = 0;

	data_field = NULL;
	zone_field = NULL;
	data_db = NULL;
	data_rec = NULL;

	num_polygons = 0;
	polygon_flag = false;
	polygon_files = NULL;
	polygon_db = NULL;
	polygons = NULL;
}

//---------------------------------------------------------
//	ZoneData destructor
//---------------------------------------------------------

ZoneData::~ZoneData (void)
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
	if (zone_field != NULL) {
		delete [] zone_field;
	}
	if (data_rec != NULL) {
		delete [] data_rec;
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
	ZoneData *exe = new ZoneData ();

	return (exe->Start_Execution (commands, control));
}
