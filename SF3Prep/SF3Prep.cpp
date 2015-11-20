//*********************************************************
//	SF3Prep.cpp - SF3 data extraction
//*********************************************************

#include "SF3Prep.hpp"

char * SF3Prep::SF3_GEOGRAPHY_FILE         = "SF3_GEOGRAPHY_FILE";
char * SF3Prep::SF3_GEOGRAPHY_FILE_x       = "SF3_GEOGRAPHY_FILE_*";
char * SF3Prep::SF3_SEGMENT_FILE_x         = "SF3_SEGMENT_FILE_*";
char * SF3Prep::SF3_SUMMARY_LEVEL          = "SF3_SUMMARY_LEVEL";
char * SF3Prep::STATE_COUNTY_LIST          = "STATE_COUNTY_LIST";
char * SF3Prep::ZONE_DATA_FILE             = "ZONE_DATA_FILE";
char * SF3Prep::ZONE_DATA_FORMAT           = "ZONE_DATA_FILE_*";
char * SF3Prep::NEW_ZONE_DATA_FILE         = "NEW_ZONE_DATA_FILE";
char * SF3Prep::NEW_ZONE_DATA_FORMAT       = "NEW_ZONE_DATA_FORMAT";
char * SF3Prep::ZONE_DATA_FIELD_RANGE_x    = "ZONE_DATA_FIELD_RANGE_*";

#include "SF3_Fields.hpp"

//---------------------------------------------------------
//	SF3Prep constructor
//---------------------------------------------------------

SF3Prep::SF3Prep (void) : Execution_Service (), Db_Code ()
{
	Program ("SF3Prep");
	Version ("4.0.1");
	Title ("SF3 Data Extraction");

	char *keys [] = {
		SF3_GEOGRAPHY_FILE,
		SF3_GEOGRAPHY_FILE_x,
		SF3_SEGMENT_FILE_x,
		SF3_SUMMARY_LEVEL,
		STATE_COUNTY_LIST,
		ZONE_DATA_FILE,
		ZONE_DATA_FORMAT,
		NEW_ZONE_DATA_FILE,
		NEW_ZONE_DATA_FORMAT,
		ZONE_DATA_FIELD_RANGE_x,
		NULL
	};

	Key_List (keys);
	proj_service.Add_Keys ();

	input_flag = select_flag = type_flag = false;
	nrec = zone_in = zone_out = 0;

	sum_level = 90;

	sf3_group.Initialize (sizeof (SF3_GROUP), 10);
	state_county.Initialize (sizeof (STATE_COUNTY), 10);
	field_range.Initialize (sizeof (FIELD_RANGE), 10);
}

//---------------------------------------------------------
//	SF3Prep destructor
//---------------------------------------------------------

SF3Prep::~SF3Prep (void)
{
	SF3_GROUP *group;
	SEGMENT *segment;

	for (group = (SF3_GROUP *) sf3_group.First (); group; group = (SF3_GROUP *) sf3_group.Next ()) {
		if (group->segment != NULL) {
			for (segment = (SEGMENT *) group->segment->First (); segment; segment = (SEGMENT *) group->segment->Next ()) {
				if (segment->file != NULL) {
					delete segment->file;
				}
			}
			delete group->segment;
		}
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SF3Prep *exe = new SF3Prep ();

	return (exe->Start_Execution (commands, control));
}
