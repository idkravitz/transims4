//********************************************************* 
//	Snapshot_File.cpp - Snapshot File Input/Output
//*********************************************************

#include "Snapshot_File.hpp"

//-----------------------------------------------------------
//	Snapshot_File constructors
//-----------------------------------------------------------

Snapshot_File::Snapshot_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Snapshot_File::Snapshot_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Snapshot_File destructor
//-----------------------------------------------------------

Snapshot_File::~Snapshot_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Snapshot_File::Setup (void)
{
	File_Type ("Snapshot File");
	File_ID ("Snapshot");

	vehicle = time = link = dir = lane = offset = speed = vehtype = 0;
	accel = driver = pass = x = y = z = bearing = user = 0;
	location_flag = false;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Snapshot_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("TIME", STRING, TIME_BUFFER);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LANE", INTEGER, 2);
		Add_Field ("DISTANCE", DOUBLE, 8, 1);
		Add_Field ("VELOCITY", DOUBLE, 6, 1);
		Add_Field ("VEHTYPE", INTEGER, 2);
		Add_Field ("ACCELER", DOUBLE, 6, 1);
		Add_Field ("DRIVER", INTEGER, 10);
		Add_Field ("PASSENGERS", INTEGER, 4);
		Add_Field ("EASTING", DOUBLE, 12, 1);
		Add_Field ("NORTHING", DOUBLE, 12, 1);
		Add_Field ("ELEVATION", DOUBLE, 12, 1);
		Add_Field ("AZIMUTH", DOUBLE, 8, 1);
		Add_Field ("USER", INTEGER, 10);
	} else {
		Add_Field ("VEHICLE", INTEGER, 10);
		Add_Field ("TIME", STRING, TIME_BUFFER);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("DIR", INTEGER, 1);
		Add_Field ("LANE", INTEGER, 2);
		Add_Field ("OFFSET", DOUBLE, 8, 1);
		Add_Field ("SPEED", DOUBLE, 6, 1);
		Add_Field ("ACCEL", DOUBLE, 6, 1);
		Add_Field ("VEH_TYPE", INTEGER, 2);
		Add_Field ("DRIVER", INTEGER, 10);
		Add_Field ("PASSENGERS", INTEGER, 4);
		if (Location_Flag ()) {
			Add_Field ("X_COORD", DOUBLE, 12, 1);
			Add_Field ("Y_COORD", DOUBLE, 12, 1);
			Add_Field ("Z_COORD", DOUBLE, 12, 1);
			Add_Field ("BEARING", INTEGER, 4);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Snapshot_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	vehicle = Required_Field ("VEHICLE");
	time = Required_Field ("TIME");
	link = Required_Field ("LINK");
	lane = Required_Field ("LANE");
	offset = Required_Field ("OFFSET", "DISTANCE");
	speed = Required_Field ("SPEED", "VELOCITY");

	if (!vehicle || !time || !link || !lane || !offset || !speed) return (false);

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");
	accel = Optional_Field ("ACCEL", "ACCELER");
	vehtype = Optional_Field ("VEH_TYPE", "VEHTYPE");
	driver = Optional_Field ("DRIVER", "TRAVELER");
	pass = Optional_Field ("PASSENGERS", "OCCUPANCY");
	x = Optional_Field ("X_COORD", "EASTING", "X");
	y = Optional_Field ("Y_COORD", "NORTHING", "Y");
	z = Optional_Field ("Z_COORD", "ELEVATION", "Z");
	bearing = Optional_Field ("BEARING", "AZIMUTH");
	user = Optional_Field ("USER", "OTHER");

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
