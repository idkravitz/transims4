//********************************************************* 
//	Activity_File.cpp - Activity File Input/Output
//*********************************************************

#include "Activity_File.hpp"

//-----------------------------------------------------------
//	Activity_File constructors
//-----------------------------------------------------------

Activity_File::Activity_File (Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();
}

Activity_File::Activity_File (char *filename, Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Activity_File destructor
//-----------------------------------------------------------

Activity_File::~Activity_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Activity_File::Setup (void)
{
	if (File_Access () == CREATE) {
		File_Type ("New Activity File");
		File_ID ("NewActivity");
	} else {
		File_Type ("Activity File");
		File_ID ("Activity");
	}
	hhold = person = activity = purpose = priority = 0;
	st_min = st_max = end_min = end_max = time_min = time_max = 0;
	mode = vehicle = location = num_pass = limit = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Activity_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);

		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("PERSON", INTEGER, 3);
		Add_Field ("ACTIVITY", INTEGER, 5);
		Add_Field ("PURPOSE", INTEGER, 2);
		Add_Field ("PRIORITY", INTEGER, 2);
		Add_Field ("START_MIN", STRING, TIME_BUFFER);
		Add_Field ("START_MAX", STRING, TIME_BUFFER);
		Add_Field ("START_A", DOUBLE, 10, 6);
		Add_Field ("START_B", DOUBLE, 10, 6);
		Add_Field ("END_MIN", STRING, TIME_BUFFER);
		Add_Field ("END_MAX", STRING, TIME_BUFFER);
		Add_Field ("END_A", DOUBLE, 10, 6);
		Add_Field ("END_B", DOUBLE, 10, 6);
		Add_Field ("TIME_MIN", STRING, TIME_BUFFER);
		Add_Field ("TIME_MAX", STRING, TIME_BUFFER);
		Add_Field ("TIME_A", DOUBLE, 10, 6);
		Add_Field ("TIME_B", DOUBLE, 10, 6);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("NUM_LOC", INTEGER, 2);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("NUM_PASS", INTEGER, 3);
		Add_Field ("PASSENGER", INTEGER, 10);
		Add_Field ("CONSTRAINT", INTEGER, 2);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("PERSON", INTEGER, 3);
		Add_Field ("ACTIVITY", INTEGER, 5);
		Add_Field ("PURPOSE", INTEGER, 2);
		Add_Field ("PRIORITY", INTEGER, 2);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("DURATION", STRING, TIME_BUFFER);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("PASSENGERS", INTEGER, 3);
		Add_Field ("CONSTRAINT", INTEGER, 2);
	}

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Activity_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	person = Required_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	activity = Required_Field ("ACTIVITY", "ACT");
	st_min = Required_Field ("START_MIN", "START", "START_TIME", "STARTTIME");
	st_max = Required_Field ("START_MAX", "START", "START_TIME", "STARTTIME");
	end_min = Required_Field ("END_MIN", "END", "END_TIME", "ENDTIME");
	end_max = Required_Field ("END_MAX", "END", "END_TIME", "ENDTIME");
	mode = Required_Field ("MODE", "MOD");
	location = Required_Field ("LOCATION", "LOC");

	if (!hhold || !person || !activity || !st_min || !st_max || 
		!end_min || !end_max || !mode || !location) return (false);

	//---- optional fields ----

	purpose = Optional_Field ("PURPOSE", "TYPE");
	priority = Optional_Field ("PRIORITY", "LEVEL");
	time_min = Optional_Field ("TIME_MIN", "DURATION");
	time_max = Optional_Field ("TIME_MAX", "DURATION");
	vehicle = Optional_Field ("VEHICLE", "VEH", "DRIVER");
	num_pass = Optional_Field ("NUM_PASS", "PASSENGERS");
	limit = Optional_Field ("CONSTRAINT", "LIMIT");

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Activity_File::Default_Definition (void)
{
	if (Dbase_Format () == VERSION3) {
		Create_Fields ();

		return (Write_Def_Header (NULL));
	} else {
		return (false);
	}
}
