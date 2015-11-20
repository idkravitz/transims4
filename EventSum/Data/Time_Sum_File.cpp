//********************************************************* 
//	Time_Sum_File.cpp - Event Time Summary Input/Output
//*********************************************************

#include "Time_Sum_File.hpp"

//-----------------------------------------------------------
//	Time_Sum_File constructors
//-----------------------------------------------------------

Time_Sum_File::Time_Sum_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Time_Sum_File::Time_Sum_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Time_Sum_File destructor
//-----------------------------------------------------------

Time_Sum_File::~Time_Sum_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Time_Sum_File::Setup (void)
{
	File_Type ("Time Summary File");
	File_ID ("TimeSum");

	from_time = to_time = trip_start = started = start_diff = start_abs = sum_trip = trip_end = ended = 0;
	end_diff = end_abs = mid_trip = ttime = ttime_diff = ttime_abs = sum_error = avg_error = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Time_Sum_File::Create_Fields (void) 
{
	Add_Field ("FROM_TIME", STRING, 16);
	Add_Field ("TO_TIME", STRING, 16);
	Add_Field ("TRIP_START", INTEGER, 10);
	Add_Field ("STARTED", INTEGER, 10);
	Add_Field ("START_DIFF", STRING, 16);
	Add_Field ("START_ERROR", STRING, 16);
	Add_Field ("TRIP_END", INTEGER, 10);
	Add_Field ("ENDED", INTEGER, 10);
	Add_Field ("END_DIFF", STRING, 16);
	Add_Field ("END_ERROR", STRING, 16);
	Add_Field ("MID_TRIP", INTEGER, 10);
	Add_Field ("TTIME", STRING, 16);
	Add_Field ("TTIME_DIFF", STRING, 16);
	Add_Field ("TTIME_ERROR", STRING, 16);
	Add_Field ("SUM_TRIPS", INTEGER, 10);
	Add_Field ("SUM_ERROR", STRING, 16);
	Add_Field ("AVG_ERROR", STRING, 16);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Time_Sum_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	to_time = Required_Field ("TO_TIME", "TIME", "PERIOD", "END");
	ttime = Required_Field ("TTIME", "DURATION", "TRAVELTIME");

	if (!to_time || !ttime) return (false);

	//---- optional fields ----

	from_time = Optional_Field ("FROM_TIME", "START");
	trip_start = Optional_Field ("TRIP_START", "TRIPSTART", "SCHEDULE1");
	started = Optional_Field ("STARTED", "START", "ACTUAL1");
	start_diff = Optional_Field ("START_DIFF", "STARTDIFF", "DIFF1");
	start_abs = Optional_Field ("START_ERROR", "STARTERROR", "DIFF1", "START_ABS");
	trip_end = Optional_Field ("TRIP_END", "TRIPEND", "SCHEDULE2");
	ended = Optional_Field ("ENDED", "END", "ACTUAL2");
	end_diff = Optional_Field ("END_DIFF", "ENDDIFF", "DIFF2");
	end_abs = Optional_Field ("END_ERROR", "ENDERROR", "DIFF2", "END_ABS");
	mid_trip = Optional_Field ("MID_TRIP", "TRIPS");
	ttime_diff = Optional_Field ("TTIME_DIFF", "DUR_DIFF");
	ttime_abs = Optional_Field ("TTIME_ERROR", "DUR_ERROR");
	sum_trip = Optional_Field ("SUM_TRIPS", "TRIPS");
	sum_error = Optional_Field ("SUM_ERROR", "ERROR");
	avg_error = Optional_Field ("AVG_ERROR", "NORMAL");

	return (true);
}
