//********************************************************* 
//	Subzone_File.cpp - Subzone File Input/Output
//*********************************************************

#include "Subzone_File.hpp"

//-----------------------------------------------------------
//	Subzone_File constructors
//-----------------------------------------------------------

Subzone_File::Subzone_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Subzone_File::Subzone_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Subzone_File destructor
//-----------------------------------------------------------

Subzone_File::~Subzone_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Subzone_File::Setup (void)
{
	File_Type ("Subzone Data File");
	File_ID ("Subzone");

	id = zone = x = y = data = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Subzone_File::Create_Fields (void) 
{
	Add_Field ("ID", INTEGER, 10);
	Add_Field ("ZONE", INTEGER, 5);
	Add_Field ("X_COORD", DOUBLE, 14, 2);
	Add_Field ("Y_COORD", DOUBLE, 14, 2);
	Add_Field ("DATA", INTEGER, 10);
	if (Notes_Flag ()) {
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Subzone_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("ID", "SUBZONE", "RECORD", "REC_ID", "SUB_ID");
	zone = Required_Field ("ZONE", "TAZ", "ZONENUM", "ZONE_ID", "ZONEID");
	x = Required_Field ("X_COORD", "EASTING", "X");
	y = Required_Field ("Y_COORD", "NORTHING", "Y");

	if (!id || !zone || !x || !y) return (false);

	//---- optional fields ----

	data = Optional_Field ("DATA", "VALUE", "TYPE", "SIZE");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
