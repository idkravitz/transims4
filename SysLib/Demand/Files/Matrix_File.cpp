//********************************************************* 
//	Matrix_File.cpp - Matrix File Input/Output
//*********************************************************

#include "Matrix_File.hpp"

//-----------------------------------------------------------
//	Matrix_File constructors
//-----------------------------------------------------------

Matrix_File::Matrix_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Matrix_File::Matrix_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Matrix_File destructor
//-----------------------------------------------------------

Matrix_File::~Matrix_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Matrix_File::Setup (void)
{
	File_Type ("Matrix File");
	File_ID ("Matrix");

	Period_Flag (false);

	org = des = period = data = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Matrix_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);
	}
	Add_Field ("ORG", INTEGER, 5);
	Add_Field ("DES", INTEGER, 5);

	if (Period_Flag ()) {
		Add_Field ("PERIOD", INTEGER, 3);
	}
	Add_Field ("DATA", INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Matrix_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	org = Required_Field ("ORG", "ORIGIN", "FROM", "FROM_ZONE", "I");
	des = Required_Field ("DES", "DESTINATION", "TO", "TO_ZONE", "J");

	if (!org || !des) return (false);

	//---- optional fields ----

	period = Optional_Field ("PERIOD", "INTERVAL");
	period_flag = (period != 0);

	data = Optional_Field ("DATA", "TRIPS");

	if (data == 0) {
		if (Num_Fields () > 2 && org == 1 && des == 2) {
			if (period == 3) {
				data = 4;
			} else {
				data = 3;
			}
		} else {
			Required_Field ("DATA", "TRIPS");
			return (false);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Matrix_File::Default_Definition (void)
{
	if (Dbase_Format () == VERSION3) {
		Create_Fields ();

		return (Write_Def_Header (NULL));
	} else {
		return (false);
	}
}
