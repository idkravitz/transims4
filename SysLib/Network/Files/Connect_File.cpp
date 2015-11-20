//********************************************************* 
//	Connect_File.cpp - Node File Input/Output
//*********************************************************

#include "Connect_File.hpp"

//-----------------------------------------------------------
//	Connect_File constructors
//-----------------------------------------------------------

Connect_File::Connect_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Connect_File::Connect_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Connect_File destructor
//-----------------------------------------------------------

Connect_File::~Connect_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Connect_File::Setup (void)
{
	File_Type ("Lane Connectivity File");
	File_ID ("Connect");

	node = in_link = out_link = in_low = in_high = out_low = out_high = 0;
	type = penalty = speed = capacity = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Connect_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("INLINK", INTEGER, 10);
		Add_Field ("INLANE", INTEGER, 3);
		Add_Field ("OUTLINK", INTEGER, 10);
		Add_Field ("OUTLANE", INTEGER, 3);
		Add_Field ("TYPE", STRING, 8);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("IN_LINK", INTEGER, 10);
		Add_Field ("OUT_LINK", INTEGER, 10);
		Add_Field ("IN_LOW", INTEGER, 2);
		Add_Field ("IN_HIGH", INTEGER, 2);
		Add_Field ("OUT_LOW", INTEGER, 2);
		Add_Field ("OUT_HIGH", INTEGER, 2);
		Add_Field ("TYPE", STRING, 8);
		Add_Field ("PENALTY", INTEGER, 6);
		Add_Field ("SPEED", DOUBLE, 5, 1);
		Add_Field ("CAPACITY", INTEGER, 6);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Connect_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	node = Required_Field ("NODE");
	in_link = Required_Field ("INLINK", "IN_LINK", "LINK_IN", "LINKIN");
	out_link = Required_Field ("OUTLINK", "OUT_LINK", "LINK_OUT", "LINKOUT");

	if (!node || !in_link || !out_link) return (false);

	//---- optional fields ----

	in_low = Optional_Field ("IN_LOW", "INLANE", "LOW_IN", "LANE_IN", "LANEIN");
	in_high = Optional_Field ("IN_HIGH", "INLANE", "HIGH_IN", "LANE_IN", "LANEIN");
	out_low = Optional_Field ("OUT_LOW", "OUTLANE", "LOW_OUT", "LANE_OUT", "LANEOUT");
	out_high = Optional_Field ("OUT_HIGH", "OUTLANE", "HIGH_OUT", "LANE_OUT", "LANEOUT");
	type = Optional_Field ("TYPE", "MOVEMENT", "TURN", "TURNTYPE", "TURN_TYPE");
	penalty = Optional_Field ("PENALTY", "DELAY");
	speed = Optional_Field ("SPEED", "MAX_SPD");
	capacity = Optional_Field ("CAPACITY", "SAT_FLOW");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
