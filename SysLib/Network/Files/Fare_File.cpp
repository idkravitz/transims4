//********************************************************* 
//	Fare_File.cpp - Node File Input/Output
//*********************************************************

#include "Fare_File.hpp"

//-----------------------------------------------------------
//	Fare_File constructors
//-----------------------------------------------------------

Fare_File::Fare_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Fare_File::Fare_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Fare_File destructor
//-----------------------------------------------------------

Fare_File::~Fare_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Fare_File::Setup (void)
{
	File_Type ("Transit Fare File");
	File_ID ("Fare");

	from_zone = to_zone = from_mode = to_mode = period = type = fare = 0;
	transfer = max_fare = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Fare_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("FROMZONE", STRING, 8);
		Add_Field ("TOZONE", STRING, 8);
		Add_Field ("MODE", STRING, 20);
		Add_Field ("COST", INTEGER, 6);
	} else {
		Add_Field ("FROM_ZONE", STRING, FIELD_BUFFER);
		Add_Field ("TO_ZONE", STRING, FIELD_BUFFER);
		Add_Field ("FROM_MODE", STRING, FIELD_BUFFER);
		Add_Field ("TO_MODE", STRING, FIELD_BUFFER);
		Add_Field ("PERIOD", STRING, FIELD_BUFFER);
		Add_Field ("CLASS", STRING, FIELD_BUFFER);
		Add_Field ("FARE", INTEGER, 6);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Fare_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	from_zone = Required_Field ("FROM_ZONE", "FROMZONE", "FROM", "BOARD");
	to_zone = Required_Field ("TO_ZONE", "TOZONE", "TO", "ALIGHT");
	to_mode = Required_Field ("MODE", "TO_MODE", "TOMODE");
	fare = Required_Field ("FARE", "COST");

	if (!from_zone || !to_zone || !to_mode || !fare) return (false);

	//---- optional fields ----

	from_mode = Optional_Field ("FROM_MODE", "FROMMODE");
	period = Optional_Field ("PERIOD", "TIME", "TIME_PERIOD", "TIME_RANGE");
	type = Optional_Field ("CLASS", "TYPE");

	Notes_Field (Optional_Field ("NOTES"));

	//---- old Version 4 fields ----

	transfer = Optional_Field ("TRANSFER", "XFER_FARE");
	max_fare = Optional_Field ("MAX_FARE", "MAXFARE");

	return (true);
}
