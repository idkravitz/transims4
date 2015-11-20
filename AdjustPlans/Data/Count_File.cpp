//********************************************************* 
//	Count_File.cpp - Traffic Count File Input/Output
//*********************************************************

#include "Count_File.hpp"

//-----------------------------------------------------------
//	Count_File constructors
//-----------------------------------------------------------

Count_File::Count_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Count_File::Count_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Count_File destructor
//-----------------------------------------------------------

Count_File::~Count_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Count_File::Setup (void)
{
	File_Type ("Traffic Count File");
	File_ID ("Count");

	anode = bnode = start = end = count = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Count_File::Create_Fields (void) 
{
	Add_Field ("ANODE", INTEGER, 10);
	Add_Field ("BNODE", INTEGER, 10);
	Add_Field ("START", STRING, 20);
	Add_Field ("END", STRING, 20);
	Add_Field ("COUNT", INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Count_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	anode = Required_Field ("ANODE", "NODEA", "FROM");
	bnode = Required_Field ("BNODE", "NODEB", "TO");
	start = Required_Field ("START", "START_TIME");
	end = Required_Field ("END", "END_TIME");
	count = Required_Field ("COUNT", "VOLUME");

	if (!anode || !bnode || !start || !end || !count) return (false);

	//---- optional fields ----

	return (true);
}
