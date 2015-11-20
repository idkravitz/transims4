//********************************************************* 
//	Travel_Time_File.cpp - Travel Time Input/Output
//*********************************************************

#include "Travel_Time_File.hpp"

//-----------------------------------------------------------
//	Travel_Time_File constructors
//-----------------------------------------------------------

Travel_Time_File::Travel_Time_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Travel_Time_File::Travel_Time_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Travel_Time_File destructor
//-----------------------------------------------------------

Travel_Time_File::~Travel_Time_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Travel_Time_File::Setup (void)
{
	File_Type ("Travel Time File");
	File_ID ("TravelTime");

	hhold = person = trip = leg = mode = base_start = started = start_diff = 0;
	base_end = ended = end_diff = mid_trip = ttime = 0;
	purpose = constraint = start_link = end_link = trip_start =  trip_end = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Travel_Time_File::Create_Fields (void) 
{
	Add_Field ("HOUSEHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 5);
	Add_Field ("TRIP", INTEGER, 5);
	Add_Field ("LEG", INTEGER, 2);
	Add_Field ("MODE", INTEGER, 2);
	Add_Field ("PURPOSE", INTEGER, 2);
	Add_Field ("CONSTRAINT", INTEGER, 2);
	Add_Field ("START_LINK", INTEGER, 10);
	Add_Field ("END_LINK", INTEGER, 10);
	Add_Field ("TRIP_START", STRING, 16);
	Add_Field ("BASE_START", STRING, 16);
	Add_Field ("STARTED", STRING, 16);
	Add_Field ("START_DIFF", STRING, 16);
	Add_Field ("TRIP_END", STRING, 16);
	Add_Field ("BASE_END", STRING, 16);
	Add_Field ("ENDED", STRING, 16);
	Add_Field ("END_DIFF", STRING, 16);
	Add_Field ("MID_TRIP", STRING, 16);
	Add_Field ("DURATION", STRING, 16);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Travel_Time_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	trip = Required_Field ("TRIP");

	if (!hhold || !trip) return (false);

	//---- optional fields ----

	person = Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	leg = Optional_Field ("LEG");
	mode = Optional_Field ("MODE");
	purpose = Optional_Field ("PURPOSE", "TYPE");
	constraint = Optional_Field ("CONSTRAINT", "LIMIT");
	start_link = Optional_Field ("START_LINK", "STARTLINK", "ORIGIN");
	end_link = Optional_Field ("END_LINK", "ENDLINK", "DESTINATION");
	trip_start = Optional_Field ("TRIP_START", "TRIPSTART");
	base_start = Optional_Field ("BASE_START", "BASESTART", "SCHEDULE1");
	started = Optional_Field ("STARTED", "START", "ACTUAL1");
	start_diff = Optional_Field ("START_DIFF", "STARTDIFF", "DIFF1");
	trip_end = Optional_Field ("TRIP_END", "TRIPEND");
	base_end = Optional_Field ("BASE_END", "BASEEND", "SCHEDULE2");
	ended = Optional_Field ("ENDED", "END", "ACTUAL2");
	end_diff = Optional_Field ("END_DIFF", "ENDDIFF", "DIFF2");
	mid_trip = Optional_Field ("MID_TRIP", "MIDTRIP", "TIME", "TOD");
	ttime = Optional_Field ("DURATION", "TRAVELTIME", "TTIME");

	return (true);
}
