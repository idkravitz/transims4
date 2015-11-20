//********************************************************* 
//	Shape_File.cpp - Shape Point File Input/Output
//*********************************************************

#include "Shape_File.hpp"

//-----------------------------------------------------------
//	Shape_File constructors
//-----------------------------------------------------------

Shape_File::Shape_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Shape_File::Shape_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Shape_File destructor
//-----------------------------------------------------------

Shape_File::~Shape_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Shape_File::Setup (void)
{
	File_Type ("Shape File");
	File_ID ("Shape");

	Nested (true);
	Header_Lines (2);

	z_flag = false;
	link = points = x = y = z = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Shape_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);

		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NPOINTS", INTEGER, 10);
		Add_Field ("EASTING", DOUBLE, 14, 2, -1, false, true);
		Add_Field ("NORTHING", DOUBLE, 14, 2, -1, false, true);
		if (Z_Flag ()) {
			Add_Field ("ELEVATION", DOUBLE, 14, 2, -1, false, true);
		}
	} else {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("POINTS", INTEGER, 10);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
		Add_Field ("X_COORD", DOUBLE, 14, 2, -1, false, true);
		Add_Field ("Y_COORD", DOUBLE, 14, 2, -1, false, true);
		if (Z_Flag ()) {
			Add_Field ("Z_COORD", DOUBLE, 14, 2, -1, false, true);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Shape_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	points = Required_Field ("POINTS", "NPOINTS", "NUM_POINTS");
	x = Required_Field ("X_COORD", "EASTING", "X");
	y = Required_Field ("Y_COORD", "NORTHING", "Y");

	if (!link || !points || !x || !y) return (false);

	z = Optional_Field ("Z_COORD", "ELEVATION", "Z");
	Z_Flag (z > 0);

	Notes_Field (Optional_Field ("NOTES"));
	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Shape_File::Default_Definition (void)
{
	if (Dbase_Format () == VERSION3) {
		Create_Fields ();

		return (Write_Def_Header (NULL));
	} else {
		return (false);
	}
}
