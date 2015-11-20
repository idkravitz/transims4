//********************************************************* 
//	HHRec_File.cpp - household record file input/output
//*********************************************************

#include "HHRec_File.hpp"

//-----------------------------------------------------------
//	HHRec_File constructors
//-----------------------------------------------------------

HHRec_File::HHRec_File (Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();
}

HHRec_File::HHRec_File (char *filename, Access_Type access, Format_Type format) : 
	Ext_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	HHRec_File destructor
//-----------------------------------------------------------

HHRec_File::~HHRec_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void HHRec_File::Setup (void)
{
	File_Type ("Household Record File");
	File_ID ("HHRec");

	hhold = person = record = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool HHRec_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", INTEGER, 10);
	Add_Field ("PERSON", INTEGER, 5);
	Add_Field ("RECORD", INTEGER, 5);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool HHRec_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");

	if (!hhold) return (false);

	//---- optional fields ----

	person = Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	record = Optional_Field ("RECORD", "TRIP", "ACTIVITY", "REC", "NUM");

	return (true);
}
