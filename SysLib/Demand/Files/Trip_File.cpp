//********************************************************* 
//	Trip_File.cpp - Trip File Input/Output
//*********************************************************

#include "Trip_File.hpp"

//-----------------------------------------------------------
//	Trip_File constructors
//-----------------------------------------------------------

Trip_File::Trip_File (Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();
}

Trip_File::Trip_File (char *filename, Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Trip_File destructor
//-----------------------------------------------------------

Trip_File::~Trip_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Trip_File::Setup (void)
{
	if (File_Access () == CREATE) {
		File_Type ("New Trip File");
		File_ID ("NewTrip");
	} else {
		File_Type ("Trip File");
		File_ID ("Trip");
	}
	hhold = person = trip = purpose = mode = vehicle = start = origin = arrive = destination = limit = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Trip_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 3);
	Add_Field ("TRIP", INTEGER, 5);
	Add_Field ("PURPOSE", INTEGER, 2);
	Add_Field ("MODE", INTEGER, 2);
	Add_Field ("VEHICLE", INTEGER, 10);
	Add_Field ("START", STRING, TIME_BUFFER);
	Add_Field ("ORIGIN", INTEGER, 10);
	Add_Field ("ARRIVE", STRING, TIME_BUFFER);
	Add_Field ("DESTINATION", INTEGER, 10);
	Add_Field ("CONSTRAINT", INTEGER, 2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Trip_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	trip = Required_Field ("TRIP", "TRP");
	mode = Required_Field ("MODE", "MOD");
	start = Required_Field ("START", "START_TIME", "STARTTIME", "DEPART");
	origin = Required_Field ("ORIGIN", "ORG");
	arrive = Required_Field ("END", "END_TIME", "ENDTIME", "ARRIVE");
	destination = Required_Field ("DESTINATION", "DES");

	if (!hhold || !trip || !mode || !start || !origin || !arrive || !destination) return (false);

	//---- optional fields ----

	person = Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	purpose = Optional_Field ("PURPOSE", "PUR");
	vehicle = Optional_Field ("VEHICLE", "VEH");
	limit = Optional_Field ("CONSTRAINT", "LIMIT");

	return (true);
}
