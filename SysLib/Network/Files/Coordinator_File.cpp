//********************************************************* 
//	Coordinator_File.cpp - Coordinator File Input/Output
//*********************************************************

#include "Coordinator_File.hpp"

//-----------------------------------------------------------
//	Coordinator_File constructors
//-----------------------------------------------------------

Coordinator_File::Coordinator_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Coordinator_File::Coordinator_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Coordinator_File destructor
//-----------------------------------------------------------

Coordinator_File::~Coordinator_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Coordinator_File::Setup (void)
{
	File_Type ("Signal Coordinator File");
	File_ID ("Coordinator");

	id = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Coordinator_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("TYPE", STRING, 10);
		Add_Field ("ALGORITHM", STRING, 10);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("ID", INTEGER, 10);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Coordinator_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("ID", "NODE");

	if (!id) return (false);

	//---- optional fields ----

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
