//********************************************************* 
//	Problem_File.cpp - Problem File Input/Output
//*********************************************************

#include "Problem_File.hpp"

//-----------------------------------------------------------
//	Problem_File constructors
//-----------------------------------------------------------

Problem_File::Problem_File (Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();
}

Problem_File::Problem_File (char *filename, Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Problem_File destructor
//-----------------------------------------------------------

Problem_File::~Problem_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Problem_File::Setup (void)
{
	File_Type ("Problem File");
	File_ID ("Problem");

	program_code = 0;
	hhold = person = trip = mode = problem = start = origin = arrive = destination = 0;
	vehicle = time = link = dir = lane = offset = survey = route = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Problem_File::Create_Fields (void) 
{
	if (program_code == POPSYN_CODE) {			//---- population synthesizer ----
		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("PERSON", INTEGER, 3);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("PROBLEM", INTEGER, 2);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("VEHICLE", INTEGER, 10);
	} else if (program_code == ACTGEN_CODE) {	//---- activity generator ----
		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("PERSON", INTEGER, 3);
		Add_Field ("ACTIVITY", INTEGER, 5);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("PROBLEM", INTEGER, 2);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("SURVEY", INTEGER, 10);
	} else {									//---- router / microsimulator ----
		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("PERSON", INTEGER, 3);
		Add_Field ("TRIP", INTEGER, 5);
		Add_Field ("LEG", INTEGER, 3);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("PROBLEM", INTEGER, 2);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("ORIGIN", INTEGER, 10);
		Add_Field ("ARRIVE", STRING, TIME_BUFFER);
		Add_Field ("DESTINATION", INTEGER, 10);

		if (program_code == MSIM_CODE) {		//---- microsimulator ----
			Add_Field ("TIME", STRING, TIME_BUFFER);
			Add_Field ("LINK", INTEGER, 10);
			Add_Field ("DIR", INTEGER, 1);
			Add_Field ("LANE", INTEGER, 2);
			Add_Field ("OFFSET", DOUBLE, 6, 1);
			Add_Field ("ROUTE", INTEGER, 10);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Problem_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	mode = Required_Field ("MODE", "MOD");
	problem = Required_Field ("PROBLEM", "STATUS");

	if (!hhold || !mode || !problem) return (false);

	if (program_code == POPSYN_CODE) {
		trip = Optional_Field ("TRIP", "TRP", "ACTIVITY", "ACT");
	} else {
		trip = Required_Field ("TRIP", "TRP", "ACTIVITY", "ACT");
		if (!trip) return (false);
	}

	//---- optional fields ----

	person = Optional_Field ("PERSON", "PER");
	leg = Optional_Field ("LEG");
	start = Optional_Field ("START", "START_TIME");
	origin = Optional_Field ("ORIGIN", "ORG", "LOCATION", "LOC");
	arrive = Optional_Field ("ARRIVE", "END", "END_TIME");
	destination = Optional_Field ("DESTINATION", "DES");
	vehicle = Optional_Field ("VEHICLE", "VEH");
	time = Optional_Field ("TIME", "TOD");
	link = Optional_Field ("LINK");
	dir = Optional_Field ("DIR", "NODE");
	lane = Optional_Field ("LANE");
	offset = Optional_Field ("OFFSET");
	survey = Optional_Field ("SURVEY", "SURVEY_ID", "REC_ID", "SURVEYID", "RECID");
	route = Optional_Field ("ROUTE", "LINE", "TRANSIT");

	if (dir > 0) {
		if (Optional_Field ("DIR") == 0) {
			LinkDir_Type (LINK_NODE);
		} else {
			LinkDir_Type (LINK_DIR);
		}
	} else {
		LinkDir_Type (LINK_SIGN);
	}
	return (true);
}
