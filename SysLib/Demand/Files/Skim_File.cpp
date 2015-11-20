//********************************************************* 
//	Skim_File.cpp - Skim File Input/Output
//*********************************************************

#include "Skim_File.hpp"

//-----------------------------------------------------------
//	Skim_File constructors
//-----------------------------------------------------------

Skim_File::Skim_File (Access_Type access, Format_Type format) : 
	Matrix_File (access, format)
{
	Setup ();
}

Skim_File::Skim_File (char *filename, Access_Type access, Format_Type format) : 
	Matrix_File (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Skim_File destructor
//-----------------------------------------------------------

Skim_File::~Skim_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Skim_File::Setup (void)
{
	Matrix_File::Setup ();

	File_Type ("Zone Skim File");
	File_ID ("Skim");

	transit_flag = drive_flag = other_flag = length_flag = true;
	total_flag = false;

	mode = count = walk = wait = transit = drive = other = length = cost = 0;
	time = variance = total = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Skim_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);
		Add_Field ("ORG", INTEGER, 5);
		Add_Field ("DES", INTEGER, 5);
		Add_Field ("MODE", INTEGER, 2);
		Add_Field ("INTERVAL", INTEGER, 3);
		Add_Field ("TIME", DOUBLE, 16, 6);
		Add_Field ("COUNT", INTEGER, 10);
		Add_Field ("VARIANCE", DOUBLE, 16, 6);
	} else {
		Add_Field ("ORG", INTEGER, 5);
		Add_Field ("DES", INTEGER, 5);
		Add_Field ("PERIOD", INTEGER, 3);

		if (total_flag) {
			Add_Field ("TIME", INTEGER, 6);
			if (length_flag) {
				Add_Field ("LENGTH", INTEGER, 8);
			}
		} else {
			Add_Field ("COUNT", INTEGER, 5);
			Add_Field ("WALK", INTEGER, 5);

			if (transit_flag) {
				Add_Field ("WAIT", INTEGER, 5);
				Add_Field ("TRANSIT", INTEGER, 5);
			}
			if (drive_flag) {
				Add_Field ("DRIVE", INTEGER, 5);
			}
			if (other_flag) {
				Add_Field ("OTHER", INTEGER, 5);
			}
			if (length_flag) {
				Add_Field ("LENGTH", INTEGER, 8);
			}
			if (transit_flag || drive_flag) {
				Add_Field ("COST", INTEGER, 5);
			}
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Skim_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	if (!Matrix_File::Set_Field_Numbers ()) return (false);

	//---- optional fields ----

	mode = Optional_Field ("MODE", "TYPE");
	count = Optional_Field ("COUNT", "NUMBER");
	walk = Optional_Field ("WALK", "WALK_TIME");
	wait = Optional_Field ("WAIT", "WAIT_TIME");
	transit = Optional_Field ("TRANSIT", "TRANSIT_TIME");
	drive = Optional_Field ("DRIVE", "DRIVE_TIME");
	other = Optional_Field ("OTHER", "OTHER_TIME");
	length = Optional_Field ("LENGTH", "DISTANCE");
	cost = Optional_Field ("COST", "FARE");
	time = Optional_Field ("TIME", "UTILITY");
	variance = Optional_Field ("VARIANCE", "RANGE");
	total = Optional_Field ("TOTAL", "TIME");

	return (true);
}
