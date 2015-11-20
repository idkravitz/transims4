//********************************************************* 
//	Vehicle_File.cpp - Vehicle File Input/Output
//*********************************************************

#include "Vehicle_File.hpp"

//-----------------------------------------------------------
//	Vehicle_File constructors
//-----------------------------------------------------------

Vehicle_File::Vehicle_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Vehicle_File::Vehicle_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Vehicle_File destructor
//-----------------------------------------------------------

Vehicle_File::~Vehicle_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Vehicle_File::Setup (void)
{
	File_Type ("Vehicle File");
	File_ID ("Vehicle");

	vehicle = hhold = location = type = sub_type = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Vehicle_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("HHID", INTEGER, 10);
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("VEHTYPE", INTEGER, 2);
		Add_Field ("VSUBTYPE", INTEGER, 2);
	} else {
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("HHOLD", INTEGER, 10);
		Add_Field ("LOCATION", INTEGER, 10);
		Add_Field ("TYPE", INTEGER, 2);
		Add_Field ("SUBTYPE", INTEGER, 2);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Vehicle_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	vehicle = Required_Field ("VEHICLE", "VEH_ID", "VEHID", "VEH");
	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	location = Required_Field ("LOCATION", "LOT");
	type = Required_Field ("VEHTYPE", "VEH_TYPE", "TYPE");

	if (!vehicle || !hhold || !location || !type) return (false);

	//---- optional fields ----

	sub_type = Optional_Field ("SUBTYPE", "SUB_TYPE", "VSUBTYPE");

	return (true);
}
