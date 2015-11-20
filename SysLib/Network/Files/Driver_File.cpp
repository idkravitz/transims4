//********************************************************* 
//	Driver_File.cpp - Transit Driver File Input/Output
//*********************************************************

#include "Driver_File.hpp"

//-----------------------------------------------------------
//	Driver_File constructors
//-----------------------------------------------------------

Driver_File::Driver_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Driver_File::Driver_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Driver_File destructor
//-----------------------------------------------------------

Driver_File::~Driver_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Driver_File::Setup (void)
{
	File_Type ("Transit Driver File");
	File_ID ("Driver");

	Nested (true);
	Header_Lines (2);

	route = links = veh_type = sub_type = link = dir = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Driver_File::Create_Fields (void) 
{
	Add_Field ("ROUTE", INTEGER, 10);
	Add_Field ("NLINKS", INTEGER, 10);
	Add_Field ("VEHTYPE", INTEGER, 5);
	Add_Field ("SUBTYPE", INTEGER, 2);
	if (Notes_Flag ()) {
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	}
	Add_Field ("LINK", INTEGER, 10, 0, -1, false, true);

	if (Dbase_Format () == VERSION3 || LinkDir_Type () == LINK_NODE) {
		Add_Field ("NODE", INTEGER, 10, 0, -1, false, true);
	} else if (LinkDir_Type () == LINK_DIR) {
		Add_Field ("DIR", INTEGER, 2, 0, -1, false, true);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Driver_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	route = Required_Field ("ROUTE");
	links = Required_Field ("LINKS", "NLINKS", "NUM_LINKS");
	link = Required_Field ("LINK", "LINK_DIR", "LINKDIR");

	if (!route || !links || !link) return (false);

	//---- optional fields ----

	veh_type = Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	sub_type = Optional_Field ("SUBTYPE", "SUB_TYPE");
	dir = Optional_Field ("DIR", "NODE");
	Notes_Field (Optional_Field ("NOTES"));

	if (dir > 0) {
		if (Optional_Field ("DIR") == 0) {
			LinkDir_Type (LINK_NODE);
		} else {
			LinkDir_Type (LINK_DIR);
		}
	} else {
		LinkDir_Type (LINK_SIGN);
	}
	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Driver_File::Default_Definition (void)
{
	Nested (true);
	Header_Lines (2);
	Create_Fields ();

	return (Write_Def_Header (NULL));
}
