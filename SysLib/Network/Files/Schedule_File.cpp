//********************************************************* 
//	Schedule_File.cpp - Transit Schedule File Input/Output
//*********************************************************

#include "Schedule_File.hpp"

//-----------------------------------------------------------
//	Schedule_File constructors
//-----------------------------------------------------------

Schedule_File::Schedule_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Schedule_File::Schedule_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Schedule_File destructor
//-----------------------------------------------------------

Schedule_File::~Schedule_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Schedule_File::Setup (void)
{
	File_Type ("Transit Schedule File");
	File_ID ("Schedule");

	route = time = stop = 0;
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Schedule_File::Read_Header (bool stat)
{
	bool messages = Send_Messages ();
	Send_Messages (false);

	if (!Db_Header::Read_Header (stat)) {
		if (!stat && Dbase_Format () == VERSION3) {
			Send_Messages (messages);
			if (!Create_Fields ()) return (false);
			return (Write_Def_Header (Header_Record ()));
		}
	} else if (stat && Dbase_Format () == VERSION3) {
		Record_Format (SPACE_DELIMITED);
	}
	Send_Messages (messages);
	//return (false);
	return (true);
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Schedule_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Clear_Fields ();
		Header_Lines (0);
		First_Offset (0);
		Rewind ();
		Record_Format (SPACE_DELIMITED);

		Add_Field ("ROUTE", INTEGER, 10);
		Add_Field ("TIME", INTEGER, 10);
		Add_Field ("STOP", INTEGER, 10);
	} else {
		Add_Field ("ROUTE", INTEGER, 10);
		Add_Field ("TIME", INTEGER, 10);
		Add_Field ("STOP", INTEGER, 10);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Schedule_File::Set_Field_Numbers (void)
{
	//---- optional fields ----

	route = Optional_Field ("ROUTE", "ID");
	time = Optional_Field ("TIME", "DEPART");
	stop = Optional_Field ("STOP", "STOPID");

	if (!route || !time || !stop) {
		if (route || time || stop) return (false);
		return (Create_Fields ());
	}
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
