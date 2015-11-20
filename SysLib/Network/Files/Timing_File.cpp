//********************************************************* 
//	Timing_File.cpp - Timing Plan File Input/Output
//*********************************************************

#include "Timing_File.hpp"

//-----------------------------------------------------------
//	Timing_File constructors
//-----------------------------------------------------------

Timing_File::Timing_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Timing_File::Timing_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Timing_File destructor
//-----------------------------------------------------------

Timing_File::~Timing_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Timing_File::Setup (void)
{
	File_Type ("Timing Plan File");
	File_ID ("Timing");

	timing = phase = next = minimum = maximum = extend = yellow = red = ring = group = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Timing_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("PLAN", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 2);
		Add_Field ("NEXTPHASES", STRING, 16);
		Add_Field ("GREENMIN", INTEGER, 3);
		Add_Field ("GREENMAX", INTEGER, 3);
		Add_Field ("GREENEXT", INTEGER, 3);
		Add_Field ("YELLOW", INTEGER, 2);
		Add_Field ("REDCLEAR", INTEGER, 2);
		Add_Field ("GROUPFIRST", INTEGER, 2);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
		//Add_Field ("GROUP", INTEGER, 2);
		Add_Field ("BARRIER", INTEGER, 2);
	} else {
		Add_Field ("TIMING", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 2);
		Add_Field ("NEXT_PHASE", STRING, 16);
		Add_Field ("MIN_GREEN", INTEGER, 3);
		Add_Field ("MAX_GREEN", INTEGER, 3);
		Add_Field ("EXT_GREEN", INTEGER, 3);
		Add_Field ("YELLOW", INTEGER, 2);
		Add_Field ("RED_CLEAR", INTEGER, 2);
		Add_Field ("RING", INTEGER, 2);
		//Add_Field ("GROUP", INTEGER, 2);
		Add_Field ("BARRIER", INTEGER, 2);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Timing_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	timing = Required_Field ("TIMING", "PLAN");
	phase = Required_Field ("PHASE");
	next = Required_Field ("NEXT_PHASE", "NEXTPHASES");
	minimum = Required_Field ("MIN_GREEN", "GREENMIN");

	if (!timing || !phase || !next || !minimum) return (false);

	//---- optional fields ----

	maximum = Optional_Field ("MAX_GREEN", "GREENMAX");
	extend = Optional_Field ("EXT_GREEN", "GREENEXT");
	yellow = Optional_Field ("YELLOW");
	red = Optional_Field ("RED_CLEAR", "REDCLEAR");
	ring = Optional_Field ("RING", "GROUPFIRST");
	group = Optional_Field ("GROUP", "BARRIER");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
