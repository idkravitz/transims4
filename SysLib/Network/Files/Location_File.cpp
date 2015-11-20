//********************************************************* 
//	Location_File.cpp - Activity Location File Input/Output
//*********************************************************

#include "Location_File.hpp"

//-----------------------------------------------------------
//	Location_File constructors
//-----------------------------------------------------------

Location_File::Location_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Location_File::Location_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Location_File destructor
//-----------------------------------------------------------

Location_File::~Location_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Location_File::Setup (void)
{
	File_Type ("Activity Location File");
	File_ID ("Location");

	location = link = node = offset = x = y = zone = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Location_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 2);
		Add_Field ("LAYER", STRING, 16);
		Add_Field ("EASTING", DOUBLE, 14, 2);
		Add_Field ("NORTHING", DOUBLE, 14, 2);
		Add_Field ("ELEVATION", DOUBLE, 14, 2);
		Add_Field ("TAZ", INTEGER, 10);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 1);
		Add_Field ("X_COORD", DOUBLE, 14, 1);
		Add_Field ("Y_COORD", DOUBLE, 14, 1);
		Add_Field ("ZONE", INTEGER, 10);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Location_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	location = Required_Field ("LOCATION", "ID");
	link = Required_Field ("LINK");
	node = Required_Field ("DIR", "NODE");
	offset = Required_Field ("OFFSET");
	x = Required_Field ("X_COORD", "EASTING", "X");
	y = Required_Field ("Y_COORD", "NORTHING", "Y");

	if (!location || !link || !node || !offset || !x || !y) return (false);

	//---- optional fields ----

	z = Optional_Field ("Z_COORD", "ELEVATION", "Z");
	zone = Optional_Field ("ZONE", "TAZ");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
