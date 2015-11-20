//********************************************************* 
//	Factor_File.cpp - Factor File Input/Output
//*********************************************************

#include "Factor_File.hpp"

//-----------------------------------------------------------
//	Factor_File constructors
//-----------------------------------------------------------

Factor_File::Factor_File (Access_Type access, Format_Type format) : 
	Matrix_File (access, format)
{
	Setup ();
}

Factor_File::Factor_File (char *filename, Access_Type access, Format_Type format) : 
	Matrix_File (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Factor_File destructor
//-----------------------------------------------------------

Factor_File::~Factor_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Factor_File::Setup (void)
{
	Matrix_File::Setup ();

	File_Type ("Correction Factor File");
	File_ID ("Factor");

	factor = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Factor_File::Create_Fields (void) 
{
	Add_Field ("ORG", INTEGER, 5);
	Add_Field ("DES", INTEGER, 5);

	if (Period_Flag ()) {
		Add_Field ("PERIOD", INTEGER, 3);
	}
	Add_Field ("FACTOR", DOUBLE, 16, 6);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Factor_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	if (!Matrix_File::Set_Field_Numbers ()) return (false);
	
	factor = Required_Field ("FACTOR", "FAC");

	if (!factor) return (false);

	return (true);
}
