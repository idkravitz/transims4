//********************************************************* 
//	Occupancy_File.cpp - Occupancy File Input/Output
//*********************************************************

#include "Occupancy_File.hpp"

//-----------------------------------------------------------
//	Occupancy_File constructors
//-----------------------------------------------------------

Occupancy_File::Occupancy_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Occupancy_File::Occupancy_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Occupancy_File destructor
//-----------------------------------------------------------

Occupancy_File::~Occupancy_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Occupancy_File::Setup (void)
{
	File_Type ("Occupancy File");
	File_ID ("Occupancy");

	link = dir = start = end = lane = offset = occupancy = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Occupancy_File::Create_Fields (void) 
{
	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("DIR", INTEGER, 1);
	Add_Field ("START", STRING, TIME_BUFFER);
	Add_Field ("END", STRING, TIME_BUFFER);
	Add_Field ("LANE", INTEGER, 2);
	Add_Field ("OFFSET", DOUBLE, 8, 1);
	Add_Field ("OCCUPANCY", INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Occupancy_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	end = Required_Field ("END", "ENDTIME", "END_TIME");
	lane = Required_Field ("LANE");
	offset = Required_Field ("OFFSET", "DISTANCE");

	if (!link || !end || !lane || !offset) return (false);

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");
	start = Optional_Field ("START", "STARTTIME", "START_TIME");
	occupancy = Optional_Field ("OCCUPANCY", "COUNT");

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
