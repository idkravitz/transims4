//********************************************************* 
//	Line_File.cpp - Transit Route File Input/Output
//*********************************************************

#include "Line_File.hpp"

//-----------------------------------------------------------
//	Line_File constructors
//-----------------------------------------------------------

Line_File::Line_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Line_File::Line_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Line_File destructor
//-----------------------------------------------------------

Line_File::~Line_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Line_File::Setup (void)
{
	File_Type ("Transit Route File");
	File_ID ("Route");

	Nested (true);
	Header_Lines (2);

	route = stops = mode = stop = link = node = zone = flag = name = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Line_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);
		Dbase_Format (SPACE_DELIMITED);

		Add_Field ("ROUTE", INTEGER, 10);
		Add_Field ("NSTOPS", INTEGER, 10);
		Add_Field ("MODE", STRING, 16);
		Add_Field ("STOP", INTEGER, 10, 0, -1, false, true);
		Add_Field ("LINK", INTEGER, 10, 0, -1, false, true);
		Add_Field ("NODE", INTEGER, 10, 0, -1, false, true);
		Add_Field ("ZONE", INTEGER, 10, 0, -1, false, true);
	} else {
		Add_Field ("ROUTE", INTEGER, 10);
		Add_Field ("NSTOPS", INTEGER, 10);
		Add_Field ("MODE", STRING, 16);
		if (Notes_Flag ()) {
			Add_Field ("NAME", STRING, FIELD_BUFFER);
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
		Add_Field ("STOP", INTEGER, 10, 0, -1, false, true);
		Add_Field ("ZONE", INTEGER, 10, 0, -1, false, true);
		Add_Field ("TIMEPT", INTEGER, 2, 0, -1, false, true);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Line_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	route = Required_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
	stops = Required_Field ("STOPS", "NSTOPS", "NUM_STOPS");
	mode = Required_Field ("MODE", "TYPE");
	stop = Required_Field ("STOP");

	if (!route || !mode || !stops || !stop) return (false);

	//---- optional fields ----

	link = Optional_Field ("LINK");
	node = Optional_Field ("NODE");
	zone = Optional_Field ("ZONE", "FARE_ZONE", "DISTRICT");
	flag = Optional_Field ("TIMEPT", "FLAG");
	name = Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Line_File::Default_Definition (void)
{
	Nested (true);
	Create_Fields ();

	return (Write_Def_Header (NULL));
}
