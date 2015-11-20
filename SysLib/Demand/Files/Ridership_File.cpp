//********************************************************* 
//	Ridership_File.cpp - Transit Ridership Input/Output
//*********************************************************

#include "Ridership_File.hpp"

//-----------------------------------------------------------
//	Ridership_File constructors
//-----------------------------------------------------------

Ridership_File::Ridership_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Ridership_File::Ridership_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Ridership_File destructor
//-----------------------------------------------------------

Ridership_File::~Ridership_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Ridership_File::Setup (void)
{
	File_Type ("Transit Ridership File");
	File_ID ("Ridership");

	mode = route = run = stop = schedule = time = board = alight = load = factor = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Ridership_File::Create_Fields (void) 
{
	Add_Field ("MODE", STRING, 16);
	Add_Field ("ROUTE", INTEGER, 10);
	Add_Field ("RUN", INTEGER, 5);
	Add_Field ("STOP", INTEGER, 10);
	Add_Field ("SCHEDULE", STRING, TIME_BUFFER);
	Add_Field ("TIME", STRING, TIME_BUFFER);
	Add_Field ("BOARD", INTEGER, 5);
	Add_Field ("ALIGHT", INTEGER, 5);
	Add_Field ("LOAD", INTEGER, 5);
	Add_Field ("FACTOR", DOUBLE, 5, 2);
	
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Ridership_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	route = Required_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
	run = Required_Field ("RUN", "TRIP");
	stop = Required_Field ("STOP", "STOP_ID");
	board = Required_Field ("BOARD", "ON", "GET_ON");
	alight = Required_Field ("ALIGHT", "OFF", "GET_OFF");

	if (!route || !run || !stop || !board || !alight) return (false);

	//---- optional fields ----

	mode = Optional_Field ("MODE", "TYPE");
	schedule = Optional_Field ("SCHEDULE", "DEPART");
	time = Optional_Field ("TIME", "ARRIVE", "ACTUAL");
	load = Optional_Field ("LOAD", "RIDERS", "PASSENGERS", "IN");
	factor = Optional_Field ("FACTOR", "LOAD_FACTOR", "VC");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
