//********************************************************* 
//	Detector_File.cpp - Detector File Input/Output
//*********************************************************

#include "Detector_File.hpp"

//-----------------------------------------------------------
//	Detector_File constructors
//-----------------------------------------------------------

Detector_File::Detector_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Detector_File::Detector_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Detector_File destructor
//-----------------------------------------------------------

Detector_File::~Detector_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Detector_File::Setup (void)
{
	File_Type ("Detector File");
	File_ID ("Detector");

	id = node = link = offset = length = low = high = type = coord = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Detector_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 8, 1);
		Add_Field ("LANEBEGIN", INTEGER, 2);
		Add_Field ("LANEEND", INTEGER, 2);
		Add_Field ("LENGTH", DOUBLE, 8, 1);
		Add_Field ("STYLE", STRING, 10);
		Add_Field ("COORDINATR", INTEGER, 10);
		Add_Field ("CATEGORY", STRING, 10);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("DETECTOR", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("OFFSET", DOUBLE, 8, 1);
		Add_Field ("LENGTH", DOUBLE, 8, 1);
		Add_Field ("LOW_LANE", INTEGER, 2);
		Add_Field ("HIGH_LANE", INTEGER, 2);
		Add_Field ("TYPE", STRING, 16);
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

bool Detector_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("ID", "DETECTOR");
	node = Required_Field ("NODE");
	link = Required_Field ("LINK");
	length = Required_Field ("LENGTH");
	low = Required_Field ("LOW_LANE", "LANEBEGIN");

	if (!id || !node || !link || !length || !low) return (false);

	//---- optional fields ----

	offset = Optional_Field ("OFFSET");
	high = Optional_Field ("HIGH_LANE", "LANEEND");
	type = Optional_Field ("TYPE", "STYLE");
	coord = Optional_Field ("COORDINATOR", "COORDINATR");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
