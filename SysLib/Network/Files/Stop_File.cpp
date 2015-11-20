//********************************************************* 
//	Stop_File.cpp - Transit Stop File Input/Output
//*********************************************************

#include "Stop_File.hpp"

//-----------------------------------------------------------
//	Stop_File constructors
//-----------------------------------------------------------

Stop_File::Stop_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Stop_File::Stop_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Stop_File destructor
//-----------------------------------------------------------

Stop_File::~Stop_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Stop_File::Setup (void)
{
	File_Type ("Transit Stop File");
	File_ID ("Stop");

	id = link = dir = offset = use = type = space = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Stop_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NAME", STRING, 50);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 1);
		Add_Field ("VEHICLE", STRING, FIELD_BUFFER);
		Add_Field ("STYLE", STRING, 16);
		Add_Field ("CAPACITY", INTEGER, 8);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("STOP", INTEGER, 10);
		if (Notes_Flag ()) {
			Add_Field ("NAME", STRING, 50);
		}
		Add_Field ("LINK", INTEGER, 10);

		if (LinkDir_Type () == LINK_NODE) {
			Add_Field ("NODE", INTEGER, 10);
		} else if (LinkDir_Type () == LINK_DIR) {
			Add_Field ("DIR", INTEGER, 2);
		}
		Add_Field ("OFFSET", DOUBLE, 10, 1);
		Add_Field ("USE", STRING, 40);
		Add_Field ("TYPE", STRING, 16);
		Add_Field ("SPACE", INTEGER, 4);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Stop_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("STOP", "STOP_ID", "ID");
	link = Required_Field ("LINK");
	offset = Required_Field ("OFFSET");

	if (!id || !link || !offset) return (false);

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

	use = Optional_Field ("USE", "VEHICLE");
	type = Optional_Field ("TYPE", "STYLE");
	space = Optional_Field ("CAPACITY", "SPACE");
	name = Optional_Field ("NAME", "STOP_NAME", "STNAME", "DESCRIPTION");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
