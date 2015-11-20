//********************************************************* 
//	Traveler_File.cpp - Traveler File Input/Output
//*********************************************************

#include "Traveler_File.hpp"

//-----------------------------------------------------------
//	Traveler_File constructors
//-----------------------------------------------------------

Traveler_File::Traveler_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Traveler_File::Traveler_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Traveler_File destructor
//-----------------------------------------------------------

Traveler_File::~Traveler_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Traveler_File::Setup (void)
{
	File_Type ("Traveler File");
	File_ID ("Traveler");

	traveler = vehicle = time = link = dir = offset = lane = speed = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Traveler_File::Create_Fields (void) 
{
	Add_Field ("TRAVELER", INTEGER, 10);
	Add_Field ("VEHICLE", INTEGER, 10);
	Add_Field ("TIME", STRING, TIME_BUFFER);
	Add_Field ("DISTANCE", DOUBLE, 10, 1);
	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("DIR", INTEGER, 1);
	Add_Field ("OFFSET", DOUBLE, 8, 1);
	Add_Field ("LANE", INTEGER, 2);
	Add_Field ("SPEED", DOUBLE, 6, 1);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Traveler_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	traveler = Required_Field ("TRAVELER");
	vehicle = Required_Field ("VEHICLE");
	time = Required_Field ("TIME");
	link = Required_Field ("LINK");
	lane = Required_Field ("LANE");
	speed = Required_Field ("SPEED");

	if (!traveler || !vehicle || !time || !link || !lane || !speed) return (false);

	//---- optional fields ----

	distance = Optional_Field ("DISTANCE");
	dir = Optional_Field ("DIR", "BA");
	offset = Optional_Field ("OFFSET");

	return (true);
}
