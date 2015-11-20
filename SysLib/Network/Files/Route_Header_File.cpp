//********************************************************* 
//	Route_Header_File.cpp - Route Header File Input/Output
//*********************************************************

#include "Route_Header_File.hpp"

#include "Utility.hpp"

#include <string.h>

//-----------------------------------------------------------
//	Route_Header_File constructors
//-----------------------------------------------------------

Route_Header_File::Route_Header_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Route_Header_File::Route_Header_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Route_Header_File destructor
//-----------------------------------------------------------

Route_Header_File::~Route_Header_File (void)
{
	Clear_Fields ();
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Route_Header_File::Setup (void)
{
	File_Type ("Route Header File");
	File_ID ("RouteHeader");

	time_flag = false;
	periods = route = mode = time = name;
	period = NULL;

	Custom_Header (true);
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Route_Header_File::Create_Fields (void) 
{
	int i;
	char buffer [FIELD_BUFFER];

	Add_Field ("ROUTE", INTEGER, 10);
	Add_Field ("MODE", STRING, 20);
	if (!time_flag) {
		Add_Field ("TTIME", INTEGER, 5);
	}
	if (Notes_Flag ()) {
		Add_Field ("NAME", STRING, 40);
	}
	for (i=1; i <= periods; i++) {
		str_fmt (buffer, sizeof (buffer), "HEADWAY_%d", i);
		Add_Field (buffer, INTEGER, 10);

		str_fmt (buffer, sizeof (buffer), "OFFSET_%d", i);
		Add_Field (buffer, INTEGER, 10);
		
		if (time_flag) {
			str_fmt (buffer, sizeof (buffer), "TTIME_%d", i);
			Add_Field (buffer, INTEGER, 10);
		}
	}
	if (Notes_Flag ()) {
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	}
	return (Set_Field_Numbers ());
}

bool Route_Header_File::Create_Fields (int num_periods, bool flag)
{
	Periods (num_periods);
	Time_Flag (flag);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Route_Header_File::Set_Field_Numbers (void)
{
	int i, num, num_periods;
	char buffer [FIELD_BUFFER];

	Db_Field *fld;

	route = Required_Field ("ROUTE", "LINE");
	mode = Required_Field ("MODE", "TYPE");
	
	if (route == 0 || mode == 0) return (false);

	time = Optional_Field ("TTIME", "TIME", "MIN_TIME");
	name = Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	Notes_Field (Optional_Field ("NOTES"));

	//---- scan the header fields ----

	num_periods = 0;
	time_flag = false;

	for (i=1; i <= Num_Fields (); i++) {
		fld = Field (i);
		str_cpy (buffer, sizeof (buffer), fld->Name ());
			
		if (str_cmp (buffer, "HEADWAY_", 8) == 0) {
			num = atoi (buffer + 8);
			if (num > num_periods) num_periods = num;
		} else if (str_cmp (buffer, "TTIME_", 6) == 0) {
			num = atoi (buffer + 6);
			if (num > 0) time_flag = true;
		}
	}
	Periods (num_periods);

	for (i=1; i <= periods; i++) {
		str_fmt (buffer, sizeof (buffer), "HEADWAY_%d", i);
		headway_field (i, Required_Field (buffer));

		str_fmt (buffer, sizeof (buffer), "OFFSET_%d", i);
		offset_field (i, Optional_Field (buffer));

		str_fmt (buffer, sizeof (buffer), "TTIME_%d", i);
		ttime_field (i, Optional_Field (buffer));
	}
	return (true);
}

//-----------------------------------------------------------
//	Periods
//-----------------------------------------------------------

bool Route_Header_File::Periods (int num_periods)
{
	if (periods == num_periods) return (true);

	if (period != NULL) {
		delete [] period;
		period = NULL;
	}
	period = new Period_Data [num_periods];

	if (period == NULL) return (false);

	periods = num_periods;

	memset (period, '\0', periods * sizeof (Period_Data));
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Route_Header_File::Zero_Fields (void)
{
	for (int i=1; i <= Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Route_Header_File::Clear_Fields (void)
{
	route = mode = time = name = periods = 0;
	time_flag = false;

	if (period != NULL) {
		delete [] period;
		period = NULL;
	}
}
