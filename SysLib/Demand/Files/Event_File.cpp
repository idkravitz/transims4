//********************************************************* 
//	Event_File.cpp - Event Input/Output
//*********************************************************

#include "Event_File.hpp"

//-----------------------------------------------------------
//	Event_File constructors
//-----------------------------------------------------------

Event_File::Event_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Event_File::Event_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Event_File destructor
//-----------------------------------------------------------

Event_File::~Event_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Event_File::Setup (void)
{
	File_Type ("Event File");
	File_ID ("Event");

	hhold = person = mode = trip = leg = type = schedule = actual = link = offset = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Event_File::Create_Fields (void) 
{
	Add_Field ("HOUSEHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 2);
	Add_Field ("TRIP", INTEGER, 4);
	Add_Field ("LEG", INTEGER, 2);
	Add_Field ("MODE", INTEGER, 2);
	Add_Field ("EVENT", INTEGER, 2);
	Add_Field ("SCHEDULE", STRING, TIME_BUFFER);
	Add_Field ("ACTUAL", STRING, TIME_BUFFER);
	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("OFFSET", INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Event_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	trip = Required_Field ("TRIP");
	type = Required_Field ("EVENT", "TYPE");
	schedule = Required_Field ("SCHEDULE");
	actual = Required_Field ("ACTUAL");

	if (!hhold || !trip || !schedule || !actual) return (false);

	//---- optional fields ----

	person = Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	mode = Optional_Field ("MODE");
	leg = Optional_Field ("LEG");
	link = Optional_Field ("LINK");
	offset = Optional_Field ("OFFSET");

	return (true);
}
