//********************************************************* 
//	Population_File.cpp - Household File Input/Output
//*********************************************************

#include "Population_File.hpp"

//-----------------------------------------------------------
//	Population_File constructors
//-----------------------------------------------------------

Population_File::Population_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Population_File::Population_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Population_File destructor
//-----------------------------------------------------------

Population_File::~Population_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Population_File::Setup (void)
{
	File_Type ("Population File");
	File_ID ("Population");

	hhold = person = relate = age = gender = drive = work = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Population_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 2);
	Add_Field ("RELATE", INTEGER, 2);
	Add_Field ("AGE", INTEGER, 2);
	Add_Field ("GENDER", INTEGER, 2);
	Add_Field ("WORK", INTEGER, 2);
	Add_Field ("DRIVE", INTEGER, 2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Population_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	person = Required_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");

	if (!hhold || !person) return (false);

	//---- optional fields ----

	relate = Optional_Field ("RELATE", "RELATION", "ROLE", "TYPE");
	age = Optional_Field ("AGE");
	gender = Optional_Field ("GENDER", "SEX");
	work = Optional_Field ("WORK", "WORKER");
	drive = Optional_Field ("DRIVE", "DRIVER");

	return (true);
}
