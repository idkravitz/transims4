//********************************************************* 
//	Zone_File.cpp - Zone File Input/Output
//*********************************************************

#include "Zone_File.hpp"

//-----------------------------------------------------------
//	Zone_File constructors
//-----------------------------------------------------------

Zone_File::Zone_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Zone_File::Zone_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Zone_File destructor
//-----------------------------------------------------------

Zone_File::~Zone_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Zone_File::Setup (void)
{
	File_Type ("Zone File");
	File_ID ("Zone");

	zone = x = y = area = min_x = min_y = max_x = max_y = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Zone_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("EASTING", DOUBLE, 14, 2);
		Add_Field ("NORTHING", DOUBLE, 14, 2);
		Add_Field ("ELEVATION", DOUBLE, 14, 2);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("ZONE", INTEGER, 10);
		Add_Field ("X_COORD", DOUBLE, 14, 2);
		Add_Field ("Y_COORD", DOUBLE, 14, 2);
		Add_Field ("AREA_TYPE", INTEGER, 3);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Zone_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	zone = Required_Field ("ZONE", "ZONE_ID", "ZONEID", "ZID", "ID");
	x = Required_Field ("X_COORD", "EASTING", "X");
	y = Required_Field ("Y_COORD", "NORTHING", "Y");

	if (!zone || !x || !y) return (false);

	//---- optional fields ----

	area = Optional_Field ("AREA_TYPE", "AREATYPE", "TYPE", "AT");
	min_x = Optional_Field ("MIN_X", "MINIMUM_X", "MINX", "LOW_X", "LOWX");
	min_y = Optional_Field ("MIN_Y", "MINIMUM_Y", "MINY", "LOW_Y", "LOWY");
	max_x = Optional_Field ("MAX_X", "MAXIMUM_X", "MAXX", "HIGH_X", "HIGHX");
	max_y = Optional_Field ("MAX_Y", "MAXIMUM_Y", "MAXY", "HIGH_Y", "HIGHY");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
