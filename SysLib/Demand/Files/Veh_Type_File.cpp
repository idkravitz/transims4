//********************************************************* 
//	Veh_Type_File.cpp - Vehicle Type File Input/Output
//*********************************************************

#include "Veh_Type_File.hpp"

//-----------------------------------------------------------
//	Veh_Type_File constructors
//-----------------------------------------------------------

Veh_Type_File::Veh_Type_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Veh_Type_File::Veh_Type_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Veh_Type_File destructor
//-----------------------------------------------------------

Veh_Type_File::~Veh_Type_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Veh_Type_File::Setup (void)
{
	File_Type ("Vehicle Type File");
	File_ID ("VehType");

	type = sub_type = length = max_speed = max_accel = max_decel = use = capacity = 0;
	load = unload = method = min_dwell = max_dwell = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Veh_Type_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("VEHTYPE", INTEGER, 2);
		Add_Field ("VSUBTYPE", INTEGER, 2);
		Add_Field ("MAXVEL", DOUBLE, 6, 2);
		Add_Field ("MAXACCEL", DOUBLE, 6, 2);
		Add_Field ("LENGTH", DOUBLE, 6, 1);
		Add_Field ("CAPACITY", INTEGER, 4);
	} else {
		Add_Field ("TYPE", INTEGER, 2);
		Add_Field ("SUBTYPE", INTEGER, 2);
		Add_Field ("LENGTH", DOUBLE, 6, 1);
		Add_Field ("MAX_SPEED", DOUBLE, 6, 1);
		Add_Field ("MAX_ACCEL", DOUBLE, 6, 2);
		Add_Field ("MAX_DECEL", DOUBLE, 6, 2);
		Add_Field ("USE", STRING, 20);
		Add_Field ("CAPACITY", INTEGER, 4);
		Add_Field ("LOADING", DOUBLE, 6, 2);
		Add_Field ("UNLOADING", DOUBLE, 6, 2);
		Add_Field ("METHOD", STRING, 20);
		Add_Field ("MIN_DWELL", INTEGER, 4);
		Add_Field ("MAX_DWELL", INTEGER, 4);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Veh_Type_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	type = Required_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	length = Required_Field ("LENGTH", "SIZE");
	max_speed = Required_Field ("MAX_SPEED", "MAXVEL", "MAXSPEED", "MAXSPD");
	max_accel = Required_Field ("MAX_ACCEL", "MAXACCEL");
	capacity = Required_Field ("CAPACITY", "PASSENGERS");
	
	if (!type || !length || !max_speed || !max_accel || !capacity) return (false);

	//---- optional fields ----

	sub_type = Optional_Field ("SUBTYPE", "SUB_TYPE", "VSUBTYPE");
	max_decel = Optional_Field ("MAX_DECEL", "MAXDECEL");
	use = Optional_Field ("USE", "ACCESS");
	load = Optional_Field ("LOADING", "LOAD", "BOARD", "BOARDING", "ON_RATE");
	unload = Optional_Field ("UNLOADING", "UNLOAD", "ALIGHT", "ALIGHTING", "OFF_RATE");
	method = Optional_Field ("METHOD", "LOAD_TYPE", "LOAD_METHOD");
	min_dwell = Optional_Field ("MIN_DWELL", "MINDWELL", "DWELL");
	max_dwell = Optional_Field ("MAX_DWELL", "MAXDWELL", "DWELL");

	return (true);
}
