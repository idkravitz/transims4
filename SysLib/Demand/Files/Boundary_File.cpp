//********************************************************* 
//	Boundary_File.cpp - Boundary Parking Input/Output
//*********************************************************

#include "Boundary_File.hpp"

//-----------------------------------------------------------
//	Boundary_File constructors
//-----------------------------------------------------------

Boundary_File::Boundary_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Boundary_File::Boundary_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Boundary_File destructor
//-----------------------------------------------------------

Boundary_File::~Boundary_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Boundary_File::Setup (void)
{
	File_Type ("Boundary Speed File");
	File_ID ("Boundary");

	parking = start = end = speed = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Boundary_File::Create_Fields (void) 
{
	Add_Field ("PARKING", INTEGER, 10);
	Add_Field ("START", STRING, TIME_BUFFER);
	Add_Field ("END", STRING, TIME_BUFFER);
	Add_Field ("SPEED", DOUBLE, 10, 2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Boundary_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	parking = Required_Field ("PARKING", "LOT", "ID", "PARKID", "BOUNDARY");
	start = Required_Field ("START", "START_TIME", "STARTTIME");
	end = Required_Field ("END", "END_TIME", "ENDTIME", "TIME");
	speed = Required_Field ("SPEED", "AVG_SPEED");

	if (!parking || !start || !end || !speed) return (false);

	return (true);
}
