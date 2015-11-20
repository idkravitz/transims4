//********************************************************* 
//	Lane_Use_File.cpp - Lane Use File Input/Output
//*********************************************************

#include "Lane_Use_File.hpp"

//-----------------------------------------------------------
//	Lane_Use_File constructors
//-----------------------------------------------------------

Lane_Use_File::Lane_Use_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Lane_Use_File::Lane_Use_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Lane_Use_File destructor
//-----------------------------------------------------------

Lane_Use_File::~Lane_Use_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Lane_Use_File::Setup (void)
{
	File_Type ("Lane Use File");
	File_ID ("LaneUse");

	link = dir = lane = type = use = start = end = offset = length = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Lane_Use_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("LANE", INTEGER, 3);
		Add_Field ("VEHICLE", STRING, FIELD_BUFFER);
		Add_Field ("RESTRICT", CHAR, 1);
		Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		Add_Field ("ENDTIME", STRING, TIME_BUFFER);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("DIR", INTEGER, 1);
		Add_Field ("LANES", INTEGER, 3);
		Add_Field ("USE", STRING, FIELD_BUFFER);
		Add_Field ("TYPE", CHAR, 1);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("OFFSET", DOUBLE, 10, 2);
		Add_Field ("LENGTH", DOUBLE, 10, 2);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Lane_Use_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	lane = Required_Field ("LANES", "LANE");
	use = Required_Field ("USE", "VEHICLE");

	if (!link || !lane || !use) return (false);

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");
	type = Optional_Field ("TYPE", "RESTRICT");
	start = Optional_Field ("STARTTIME", "START", "START_TIME");
	end = Optional_Field ("ENDTIME", "END", "END_TIME");
	offset = Optional_Field ("OFFSET");
	length = Optional_Field ("LENGTH");
	Notes_Field (Optional_Field ("NOTES"));

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
