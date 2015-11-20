//********************************************************* 
//	Parking_File.cpp - Parking File Input/Output
//*********************************************************

#include "Parking_File.hpp"

//-----------------------------------------------------------
//	Parking_File constructors
//-----------------------------------------------------------

Parking_File::Parking_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Parking_File::Parking_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Parking_File destructor
//-----------------------------------------------------------

Parking_File::~Parking_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Parking_File::Setup (void)
{
	File_Type ("Parking File");
	File_ID ("Parking");

	id = link = node = offset = type = use = space = start = end = hourly = daily = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Parking_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 2);
		Add_Field ("STYLE", STRING, 16);
		Add_Field ("CAPACITY", INTEGER, 6);
		Add_Field ("GENERIC", CHAR, 1);
		Add_Field ("VEHICLE", STRING, FIELD_BUFFER);
		Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		Add_Field ("ENDTIME", STRING, TIME_BUFFER);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("PARKING", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 2);
		Add_Field ("TYPE", STRING, 16);
		Add_Field ("USE", STRING, FIELD_BUFFER);
		Add_Field ("SPACE", INTEGER, 6);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("HOURLY", INTEGER, 8);
		Add_Field ("DAILY", INTEGER, 8);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Parking_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("PARKING", "ID");
	link = Required_Field ("LINK");
	node = Required_Field ("DIR", "NODE");
	offset = Required_Field ("OFFSET");
	type = Required_Field ("TYPE", "STYLE");
	use = Required_Field ("USE", "VEHICLE");

	if (!id || !link || !node || !offset || !type || !use) return (false);

	//---- optional fields ----

	space = Optional_Field ("CAPACITY", "SPACE");
	start = Optional_Field ("STARTTIME", "START", "START_TIME");
	end = Optional_Field ("ENDTIME", "END", "END_TIME");
	hourly = Optional_Field ("HOURLY", "RATE");
	daily = Optional_Field ("DAILY", "MAXIMUM");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
