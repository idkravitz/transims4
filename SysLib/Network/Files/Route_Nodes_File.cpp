//********************************************************* 
//	Route_Nodes_File.cpp - Route Nodes File Input/Output
//*********************************************************

#include "Route_Nodes_File.hpp"

//-----------------------------------------------------------
//	Route_Nodes_File constructors
//-----------------------------------------------------------

Route_Nodes_File::Route_Nodes_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Route_Nodes_File::Route_Nodes_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Route_Nodes_File destructor
//-----------------------------------------------------------

Route_Nodes_File::~Route_Nodes_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Route_Nodes_File::Setup (void)
{
	File_Type ("Route Nodes File");
	File_ID ("RouteNodes");

	dwell_flag = time_flag = speed_flag = false;
	route = node = dwell = time = speed = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Route_Nodes_File::Create_Fields (void) 
{
	Add_Field ("ROUTE", INTEGER, 10);
	Add_Field ("NODE", INTEGER, 10);

	if (dwell_flag) {
		Add_Field ("DWELL", INTEGER, 10);
	}
	if (time_flag) {
		Add_Field ("TIME", INTEGER, 10);
	}
	if (speed_flag) {
		Add_Field ("SPEED", DOUBLE, 10, 2);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Route_Nodes_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	route = Required_Field ("ROUTE", "LINE");
	node = Required_Field ("NODE", "STOP");

	if (!route || !node) return (false);

	//---- optional fields ----

	dwell = Optional_Field ("DWELL", "DWELL_TIME", "STOP_TIME", "STOPTIME");
	dwell_flag = (dwell != 0);

	time = Optional_Field ("TIME", "TTIME", "RUNTIME", "RUN_TIME");
	time_flag = (time != 0);

	speed = Optional_Field ("SPEED", "SPD", "RUNSPD", "RUN_SPD", "RUN_SPEED");
	speed_flag = (speed != 0);

	return (true);
}
