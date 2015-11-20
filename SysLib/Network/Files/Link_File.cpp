//********************************************************* 
//	Link_File.cpp - Node File Input/Output
//*********************************************************

#include "Link_File.hpp"

//-----------------------------------------------------------
//	Link_File constructors
//-----------------------------------------------------------

Link_File::Link_File (Access_Type access, Format_Type format, bool flag) : 
	Db_Header (access, format, flag)
{
	Setup (flag);
}

Link_File::Link_File (char *filename, Access_Type access, Format_Type format, bool flag) : 
	Db_Header (access, format, flag)
{
	Setup (flag);

	Open (filename);
}

//-----------------------------------------------------------
//	Link_File destructor
//-----------------------------------------------------------

Link_File::~Link_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_File::Setup (bool flag)
{
	File_Type ("Link File");
	File_ID ("Link");
	name_flag = flag;

	link = anode = bnode = length = setback_a = setback_b = bearing_a = bearing_b;
	use = type = street = 0;
	lanes_ab = left_ab = right_ab = speed_ab = fspd_ab = cap_ab = 0;
	lanes_ba = left_ba = right_ba = speed_ba = fspd_ba = cap_ba = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("NAME", STRING, 50);
		Add_Field ("NODEA", INTEGER, 10);
		Add_Field ("NODEB", INTEGER, 10);
		Add_Field ("PERMLANESA", INTEGER, 3);
		Add_Field ("PERMLANESB", INTEGER, 3);
		Add_Field ("LEFTPCKTSA", INTEGER, 3);
		Add_Field ("LEFTPCKTSB", INTEGER, 3);
		Add_Field ("RGHTPCKTSA", INTEGER, 3);
		Add_Field ("RGHTPCKTSB", INTEGER, 3);
		Add_Field ("TWOWAYTURN", CHAR, 1);
		Add_Field ("LENGTH", DOUBLE, 10, 2);
		Add_Field ("GRADE", DOUBLE, 8, 2);
		Add_Field ("SETBACKA", DOUBLE, 6, 2);
		Add_Field ("SETBACKB", DOUBLE, 6, 2);
		Add_Field ("CAPACITYA", INTEGER, 8);
		Add_Field ("CAPACITYB", INTEGER, 8);
		Add_Field ("SPEEDLMTA", DOUBLE, 6, 2);
		Add_Field ("SPEEDLMTB", DOUBLE, 6, 2);
		Add_Field ("FREESPDA", DOUBLE, 6, 2);
		Add_Field ("FREESPDB", DOUBLE, 6, 2);
		Add_Field ("FUNCTCLASS", STRING, 20);
		Add_Field ("THRUA", INTEGER, 10);
		Add_Field ("THRUB", INTEGER, 10);
		Add_Field ("COLOR", INTEGER, 2);
		Add_Field ("VEHICLE", STRING, FIELD_BUFFER);
        Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("LINK", INTEGER, 10);
		if (name_flag) {
			Add_Field ("STREET", STRING, 50);
		}
		Add_Field ("ANODE", INTEGER, 10);
		Add_Field ("BNODE", INTEGER, 10);
		Add_Field ("LENGTH", DOUBLE, 10, 2);
		Add_Field ("SETBACK_A", DOUBLE, 5, 1);
		Add_Field ("SETBACK_B", DOUBLE, 5, 1);
		Add_Field ("BEARING_A", INTEGER, 4);
		Add_Field ("BEARING_B", INTEGER, 4);
		Add_Field ("TYPE", STRING, 20);
		Add_Field ("LANES_AB", INTEGER, 2);
		Add_Field ("LEFT_AB", INTEGER, 2);
		Add_Field ("RIGHT_AB", INTEGER, 2);
		Add_Field ("SPEED_AB", DOUBLE, 5, 1);
		Add_Field ("FSPD_AB", DOUBLE, 5, 1);
		Add_Field ("CAP_AB", INTEGER, 8);
		Add_Field ("LANES_BA", INTEGER, 2);
		Add_Field ("LEFT_BA", INTEGER, 2);
		Add_Field ("RIGHT_BA", INTEGER, 2);
		Add_Field ("SPEED_BA", DOUBLE, 5, 1);
		Add_Field ("FSPD_BA", DOUBLE, 5, 1);
		Add_Field ("CAP_BA", INTEGER, 8);
		Add_Field ("USE", STRING, FIELD_BUFFER);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	anode = Required_Field ("NODEA", "ANODE", "A");
	bnode = Required_Field ("NODEB", "BNODE", "B");
	length = Required_Field ("LENGTH", "DISTANCE", "LEN");
	lanes_ab = Required_Field ("LANES_AB", "PERMLANESB", "LANESAB");
	lanes_ba = Required_Field ("LANES_BA", "PERMLANESA", "LANESBA");
	use = Required_Field ("USE", "VEHICLE");
	type = Required_Field ("TYPE", "FUNCTCLASS", "FUNCL", "CLASS");

	if (!anode || !bnode || !length || !use || !type || !lanes_ab || !lanes_ba) return (false);

	//---- at least one speed field is required ---

	speed_ab = Optional_Field ("SPEED_AB", "SPEEDLMTB", "SPD_AB", "SPEEDAB", "SPDAB");
	fspd_ab = Optional_Field ("FSPD_AB", "FREESPDB", "FREESPD_AB", "FSPDAB");
	
	if (!speed_ab && !fspd_ab) {
		speed_ab = Required_Field ("SPEED_AB", "SPEEDLMTB", "FSPD_AB", "FREESPDB");
		return (false);
	}
	speed_ba = Optional_Field ("SPEED_BA", "SPEEDLMTA", "SPD_BA", "SPEEDBA", "SPDBA");
	fspd_ba = Optional_Field ("FSPD_BA", "FREESPDA", "FREESPD_BA", "FSPDBA");

	if (!speed_ba && !fspd_ba) {
		speed_ba = Required_Field ("SPEED_BA", "SPEEDLMTA", "FSPD_BA", "FREESPDA");
		return (false);
	}

	//---- optional fields ----

	link = Optional_Field ("ID", "LINK");
	street = Optional_Field ("NAME", "STREET", "STREET_NAME", "ST_NAME", "STNAME");
	name_flag = (street != 0);

	cap_ab = Optional_Field ("CAP_AB", "CAPACITYB", "CAPACITY_AB", "CAPABITY_A");
	cap_ba = Optional_Field ("CAP_BA", "CAPACITYA", "CAPACITY_BA", "CAPACITY_B");

	left_ab = Optional_Field ("LEFT_AB", "LEFTPCKTSB", "LEFTAB");
	right_ab = Optional_Field ("RIGHT_AB", "RGHTPCKTSB", "RIGHTAB");

	left_ba = Optional_Field ("LEFT_BA", "LEFTPCKTSA", "LEFTBA");
	right_ba = Optional_Field ("RIGHT_BA", "RGHTPCKTSA", "RIGHTBA");

	setback_a = Optional_Field ("SETBACK_A", "SETBACKA");
	setback_b = Optional_Field ("SETBACK_B", "SETBACKB");
	
	bearing_a = Optional_Field ("BEARING_A", "BEARINGA");
	bearing_b = Optional_Field ("BEARING_B", "BEARINGB");

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
