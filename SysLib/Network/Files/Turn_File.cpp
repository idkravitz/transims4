//********************************************************* 
//	Turn_File.cpp - Turn Prohibition File Input/Output
//*********************************************************

#include "Turn_File.hpp"

//-----------------------------------------------------------
//	Turn_File constructors
//-----------------------------------------------------------

Turn_File::Turn_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Turn_File::Turn_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Turn_File destructor
//-----------------------------------------------------------

Turn_File::~Turn_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Turn_File::Setup (void)
{
	File_Type ("Turn Prohibition File");
	File_ID ("Turn");

	node = in_link = out_link = start = end = in_node = out_node = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Turn_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("INLINK", INTEGER, 10);
		Add_Field ("OUTLINK", INTEGER, 10);
		Add_Field ("STARTTIME", STRING, TIME_BUFFER);
		Add_Field ("ENDTIME", STRING, TIME_BUFFER);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("IN_LINK", INTEGER, 10);
		Add_Field ("OUT_LINK", INTEGER, 10);
		Add_Field ("START", STRING, TIME_BUFFER);
		Add_Field ("END", STRING, TIME_BUFFER);
		Add_Field ("USE", STRING, FIELD_BUFFER);
		Add_Field ("PENALTY", INTEGER, 6);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Turn_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE", "AT", "THRU");

	if (!node) return (false);

	in_node = Optional_Field ("FROM", "FROM_NODE", "FROMNODE");
	out_node = Optional_Field ("TO", "TO_NODE", "TONODE");

	if (in_node && out_node) {
		in_link = Optional_Field ("INLINK", "IN_LINK", "LINK_IN", "LINKIN");
		out_link = Optional_Field ("OUTLINK", "OUT_LINK", "LINK_OUT", "LINKOUT");
	} else {
		in_link = Required_Field ("INLINK", "IN_LINK", "LINK_IN", "LINKIN");
		out_link = Required_Field ("OUTLINK", "OUT_LINK", "LINK_OUT", "LINKOUT");
	}
	start = Optional_Field ("STARTTIME", "START");
	end = Optional_Field ("ENDTIME", "END");
	use = Optional_Field ("USE", "VEHICLE");
	penalty = Optional_Field ("PENALTY", "DELAY");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
