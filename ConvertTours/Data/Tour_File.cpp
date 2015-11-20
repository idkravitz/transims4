//********************************************************* 
//	Tour_File.cpp - Tour File Input/Output
//*********************************************************

#include "Tour_File.hpp"

//-----------------------------------------------------------
//	Tour_File constructors
//-----------------------------------------------------------

Tour_File::Tour_File (Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();
}

Tour_File::Tour_File (char *filename, Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Tour_File destructor
//-----------------------------------------------------------

Tour_File::~Tour_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Tour_File::Setup (void)
{
	if (File_Access () == CREATE) {
		File_Type ("New Tour Data File");
		File_ID ("NewTour");
	} else {
		File_Type ("Tour Data File");
		File_ID ("Tour");
	}
	hhold = person = tour = purpose = mode = origin = destination = stop_out = stop_in = start = end = group = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Tour_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 3);
	Add_Field ("TOUR", INTEGER, 5);
	Add_Field ("PURPOSE", INTEGER, 2);
	Add_Field ("MODE", INTEGER, 2);
	Add_Field ("ORIGIN", INTEGER, 5);
	Add_Field ("DESTINATION", INTEGER, 5);
	Add_Field ("STOP_OUT", INTEGER, 5);
	Add_Field ("STOP_IN", INTEGER, 5);
	Add_Field ("START", INTEGER, 5);
	Add_Field ("RETURN", INTEGER, 5);
	Add_Field ("GROUP", INTEGER, 2);
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Tour_File::Set_Field_Numbers (void)
{
	//---- optional fields ----

	hhold = Optional_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	person = Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	tour = Optional_Field ("TOUR", "TOUR_ID");
	purpose = Optional_Field ("PURPOSE", "PUR");
	mode = Optional_Field ("MODE", "MOD");
	origin = Optional_Field ("ORIGIN", "ORG");
	destination = Optional_Field ("DESTINATION", "DES");
	stop_out = Optional_Field ("STOP_OUT");
	stop_in = Optional_Field ("STOP_IN");
	start = Optional_Field ("START", "START_TIME", "STARTTIME", "DEPART");
	end = Optional_Field ("RETURN", "RETURN_TIME", "END", "END_TIME", "ENDTIME");
	group = Optional_Field ("GROUP", "NUMBER", "PURPOSE");

	return (true);
}
