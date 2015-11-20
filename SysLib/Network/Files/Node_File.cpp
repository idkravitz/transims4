//********************************************************* 
//	Node_File.cpp - Node File Input/Output
//*********************************************************

#include "Node_File.hpp"

//-----------------------------------------------------------
//	Node_File constructors
//-----------------------------------------------------------

Node_File::Node_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Node_File::Node_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Node_File destructor
//-----------------------------------------------------------

Node_File::~Node_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Node_File::Setup (void)
{
	File_Type ("Node File");
	File_ID ("Node");

	node = x = y = z = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Node_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("EASTING", DOUBLE, 14, 2);
		Add_Field ("NORTHING", DOUBLE, 14, 2);
		Add_Field ("ELEVATION", DOUBLE, 14, 2);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("X_COORD", DOUBLE, 14, 2);
		Add_Field ("Y_COORD", DOUBLE, 14, 2);
		Add_Field ("Z_COORD", DOUBLE, 14, 2);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Node_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE", "ID");
	x = Required_Field ("X_COORD", "EASTING", "X");
	y = Required_Field ("Y_COORD", "NORTHING", "Y");

	if (!node || !x || !y) return (false);

	//---- optional fields ----

	z = Optional_Field ("Z_COORD", "ELEVATION", "Z");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
