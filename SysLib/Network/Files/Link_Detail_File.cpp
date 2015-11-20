//********************************************************* 
//	Link_Detail_File.cpp - Link Detail File Input/Output
//*********************************************************

#include "Link_Detail_File.hpp"

//-----------------------------------------------------------
//	Link_Detail_File constructors
//-----------------------------------------------------------

Link_Detail_File::Link_Detail_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Link_Detail_File::Link_Detail_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Link_Detail_File destructor
//-----------------------------------------------------------

Link_Detail_File::~Link_Detail_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Detail_File::Setup (void)
{
	File_Type ("Link Detail File");
	File_ID ("Detail");

	link = dir = control = left = left_thru = thru = right_thru = right = use = lanes = period = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Detail_File::Create_Fields (void) 
{
	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("DIR", INTEGER, 1);
	Add_Field ("CONTROL", STRING, 16);
	Add_Field ("LEFT", INTEGER, 2);
	Add_Field ("LEFT_THRU", INTEGER, 2);
	Add_Field ("THRU", INTEGER, 2);
	Add_Field ("RIGHT_THRU", INTEGER, 2);
	Add_Field ("RIGHT", INTEGER, 2);
	Add_Field ("USE_TYPE", STRING, FIELD_BUFFER);
	Add_Field ("USE_LANES", INTEGER, 2);
	Add_Field ("USE_PERIOD", STRING, FIELD_BUFFER);
	if (Notes_Flag ()) {
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Detail_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	dir = Optional_Field ("DIR");

	if (!link || !dir) return (false);

	//---- optional fields ----

	control = Optional_Field ("CONTROL", "WARRANT");
	left = Optional_Field ("LEFT", "LEFT_LANES", "LEFT_TURN");
	left_thru = Optional_Field ("LEFT_THRU");
	thru = Optional_Field ("THRU", "THRU_LANES");
	right_thru = Optional_Field ("RIGHT_THRU");
	right = Optional_Field ("RIGHT", "RIGHT_LANES", "RIGHT_TURN");
	use = Optional_Field ("USE_TYPE", "USE");
	lanes = Optional_Field ("USE_LANES", "LANES");
	period = Optional_Field ("USE_PERIOD", "PERIOD");

	Notes_Field (Optional_Field ("NOTES"));
	return (true);
}
