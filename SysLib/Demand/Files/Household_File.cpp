//********************************************************* 
//	Household_File.cpp - Household File Input/Output
//*********************************************************

#include "Household_File.hpp"

//-----------------------------------------------------------
//	Household_File constructors
//-----------------------------------------------------------

Household_File::Household_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Household_File::Household_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Household_File destructor
//-----------------------------------------------------------

Household_File::~Household_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Household_File::Setup (void)
{
	File_Type ("Household File");
	File_ID ("Household");

	hhold = location = persons = workers = vehicles = type = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Household_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", INTEGER, 10);
	Add_Field ("LOCATION", INTEGER, 10);
	Add_Field ("PERSONS", INTEGER, 2);
	Add_Field ("WORKERS", INTEGER, 2);
	Add_Field ("VEHICLES", INTEGER, 2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Household_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	location = Required_Field ("LOCATION", "ZONE");

	if (!hhold || !location) return (false);

	//---- optional fields ----

	persons = Optional_Field ("PERSONS", "HHSIZE");
	workers = Optional_Field ("WORKERS", "JOBS");
	vehicles = Optional_Field ("VEHICLES", "AUTOS");
	type = Optional_Field ("TYPE");

	return (true);
}
