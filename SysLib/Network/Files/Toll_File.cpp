//********************************************************* 
//	Toll_File.cpp - Toll File Input/Output
//*********************************************************

#include "Toll_File.hpp"

//-----------------------------------------------------------
//	Toll_File constructors
//-----------------------------------------------------------

Toll_File::Toll_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Toll_File::Toll_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Toll_File destructor
//-----------------------------------------------------------

Toll_File::~Toll_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Toll_File::Setup (void)
{
	File_Type ("Toll File");
	File_ID ("Toll");

	link = dir = start = end = use = toll = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Toll_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		Add_Field ("ENDTIME", STRING, TIME_BUFFER);
		Add_Field ("VEHICLE", STRING, FIELD_BUFFER);
		Add_Field ("TOLL", INTEGER, 6);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("LINK", INTEGER, 10);

		if (LinkDir_Type () == LINK_NODE) {
			Add_Field ("NODE", INTEGER, 10);
		} else if (LinkDir_Type () == LINK_DIR) {
			Add_Field ("DIR", INTEGER, 2);
		}
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("USE", STRING, FIELD_BUFFER);
		Add_Field ("TOLL", INTEGER, 6);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Toll_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	toll = Required_Field ("TOLL", "COST", "CENTS");

	if (!link || !toll) return (false);

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");

	if (dir > 0) {
		if (Optional_Field ("DIR") == 0) {
			LinkDir_Type (LINK_NODE);
		} else {
			LinkDir_Type (LINK_DIR);
		}
	} else {
		LinkDir_Type (LINK_SIGN);
	}
	start = Optional_Field ("STARTTIME", "START", "START_TIME");
	end = Optional_Field ("ENDTIME", "END", "END_TIME");
	use = Optional_Field ("USE", "VEHICLE");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
