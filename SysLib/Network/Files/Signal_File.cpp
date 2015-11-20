//********************************************************* 
//	Signal_File.cpp - Signal File Input/Output
//*********************************************************

#include "Signal_File.hpp"

//-----------------------------------------------------------
//	Signal_File constructors
//-----------------------------------------------------------

Signal_File::Signal_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Signal_File::Signal_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Signal_File destructor
//-----------------------------------------------------------

Signal_File::~Signal_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Signal_File::Setup (void)
{
	File_Type ("Signalized Node File");
	File_ID ("Signal");

	node = type = timing = offset = start = coord = ring = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Signal_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("TYPE", STRING, 10);
		Add_Field ("PLAN", INTEGER, 10);
		Add_Field ("OFFSET", INTEGER, 4);
		Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		Add_Field ("COORDINATR", INTEGER, 10);
		Add_Field ("RING", STRING, 10);
		Add_Field ("ALGORITHM", STRING, 16);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("TIMING", INTEGER, 10);
		Add_Field ("TYPE", STRING, 10);
		Add_Field ("RINGS", STRING, 10);
		Add_Field ("OFFSET", INTEGER, 4);
		Add_Field ("COORDINATOR", INTEGER, 10);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Signal_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	node = Required_Field ("NODE");

	if (!node) return (false);

	//---- optional fields ----

	type = Optional_Field ("TYPE");
	timing = Optional_Field ("TIMING", "PLAN");
	offset = Optional_Field ("OFFSET");
	start = Optional_Field ("START", "STARTTIME", "START_TIME");
	coord = Optional_Field ("COORDINATOR", "COORDINATR");
	ring = Optional_Field ("RING", "RINGS");

	Notes_Field (Optional_Field ("NOTES"));
	return (true);
}
