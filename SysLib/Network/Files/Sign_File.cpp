//********************************************************* 
//	Sign_File.cpp - Sign File Input/Output
//*********************************************************

#include "Sign_File.hpp"

//-----------------------------------------------------------
//	Sign_File constructors
//-----------------------------------------------------------

Sign_File::Sign_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Sign_File::Sign_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Sign_File destructor
//-----------------------------------------------------------

Sign_File::~Sign_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Sign_File::Setup (void)
{
	File_Type ("Unsignalized Node File");
	File_ID ("Sign");

	node = link = sign = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Sign_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("INLINK", INTEGER, 10);
		Add_Field ("SIGN", STRING, 10);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);
	} else {
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("SIGN", STRING, 10);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Sign_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE");
	link = Required_Field ("INLINK", "LINK", "LINK_IN");
	sign = Required_Field ("SIGN");

	if (!node || !link || !sign) return (false);

	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
