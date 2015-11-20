//********************************************************* 
//	Access_File.cpp - Process Link File Input/Output
//*********************************************************

#include "Access_File.hpp"

//-----------------------------------------------------------
//	Access_File constructors
//-----------------------------------------------------------

Access_File::Access_File (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();
}

Access_File::Access_File (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Header (access, format, notes_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Access_File destructor
//-----------------------------------------------------------

Access_File::~Access_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Access_File::Setup (void)
{
	File_Type ("Process Link File");
	File_ID ("Access");

	id = from_id = to_id = from_type = to_type = time = cost = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Access_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("ID", INTEGER, 10);
		Add_Field ("FROMID", INTEGER, 10);
		Add_Field ("FROMTYPE", STRING, 10);
		Add_Field ("TOID", INTEGER, 10);
		Add_Field ("TOTYPE", STRING, 10);
		Add_Field ("DELAY", DOUBLE, 14, 2);
		Add_Field ("COST", DOUBLE, 14, 2);
		Add_Field ("NOTES", STRING, FIELD_BUFFER);		
	} else {
		Add_Field ("ACCESS", INTEGER, 10);
		Add_Field ("FROM_ID", INTEGER, 10);
		Add_Field ("FROM_TYPE", STRING, 10);
		Add_Field ("TO_ID", INTEGER, 10);
		Add_Field ("TO_TYPE", STRING, 10);
		Add_Field ("TIME", INTEGER, 6);
		Add_Field ("COST", INTEGER, 6);
		if (Notes_Flag ()) {
			Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Access_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	id = Required_Field ("ACCESS", "ID");
	from_id = Required_Field ("FROMID", "FROM_ID");
	to_id = Required_Field ("TOID", "TO_ID");
	from_type = Required_Field ("FROMTYPE", "FROM_TYPE");
	to_type = Required_Field ("TOTYPE", "TO_TYPE");

	if (!id || !from_id || !to_id || !from_type || !to_type) return (false);

	//---- optional fields ----

	time = Optional_Field ("DELAY", "TIME");
	cost = Optional_Field ("COST");
	Notes_Field (Optional_Field ("NOTES"));

	return (true);
}
