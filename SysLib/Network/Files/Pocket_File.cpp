//********************************************************* 
//	Pocket_File.cpp - Pocket Lane File Input/Output
//*********************************************************

#include "Pocket_File.hpp"

//-----------------------------------------------------------
//	Pocket_File constructors
//-----------------------------------------------------------

Pocket_File::Pocket_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Pocket_File::Pocket_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Pocket_File destructor
//-----------------------------------------------------------

Pocket_File::~Pocket_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Pocket_File::Setup (void)
{
	File_Type ("Pocket Lane File");
	File_ID ("Pocket");

	id = link = node = offset = lane = type = length = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Pocket_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 1);
		Add_Field ("LANE", INTEGER, 10);
		Add_Field ("STYLE", CHAR, 1);
		Add_Field ("LENGTH", DOUBLE, 10, 1);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("POCKET", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 10, 1);
		Add_Field ("LANES", INTEGER, 10);
		Add_Field ("TYPE", CHAR, 1);
		Add_Field ("LENGTH", DOUBLE, 10, 1);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Pocket_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	id = Required_Field ("POCKET", "ID");
	link = Required_Field ("LINK");
	node = Required_Field ("DIR", "NODE");
	offset = Required_Field ("OFFSET");
	lane = Required_Field ("LANES", "LANE");
	type = Required_Field ("TYPE", "STYLE");
	length = Required_Field ("LENGTH");

	if (!id || !link || !node || !offset || !lane || !type || !length) return (false);

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
