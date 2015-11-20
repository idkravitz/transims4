//********************************************************* 
//	Phasing_File.cpp - Phasing Plan File Input/Output
//*********************************************************

#include "Phasing_File.hpp"

//-----------------------------------------------------------
//	Phasing_File constructors
//-----------------------------------------------------------

Phasing_File::Phasing_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Phasing_File::Phasing_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Phasing_File destructor
//-----------------------------------------------------------

Phasing_File::~Phasing_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Phasing_File::Setup (void)
{
	File_Type ("Phasing Plan File");
	File_ID ("Phasing");

	node = timing = phase = in_link = out_link = protect = detectors = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Phasing_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("PLAN", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 3);
		Add_Field ("INLINK", INTEGER, 10);
		Add_Field ("OUTLINK", INTEGER, 10);
		Add_Field ("PROTECTION", STRING, 10);
		Add_Field ("DETECTORS", STRING, FIELD_BUFFER);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("TIMING", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 3);
		Add_Field ("IN_LINK", INTEGER, 10);
		Add_Field ("OUT_LINK", INTEGER, 10);
		Add_Field ("PROTECTION", STRING, 10);
		Add_Field ("DETECTORS", STRING, FIELD_BUFFER);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Phasing_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	node = Required_Field ("NODE");
	timing = Required_Field ("TIMING", "PLAN");
	phase = Required_Field ("PHASE");
	in_link = Required_Field ("IN_LINK", "INLINK", "LINK_IN", "LINKIN");
	out_link = Required_Field ("OUT_LINK", "OUTLINK", "LINK_OUT", "LINKOUT");

	if (!node || !timing || !phase || !in_link || !out_link) return (false);

	//---- optional fields ----

	protect = Optional_Field ("PROTECTION");
	detectors = Optional_Field ("DETECTORS");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
